/*
 * Copyright (C) 2012 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 *
 * Released according to the GNU GPL, version 2 or any later version.
 *
 * This work is part of the White Rabbit project, a research effort led
 * by CERN, the European Institute for Nuclear Research.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/fmc.h>
#include "for-2.6.24.h"

static int fmc_check_version(unsigned long version, const char *name)
{
	if (__FMC_MAJOR(version) != FMC_MAJOR) {
		pr_err("%s: \"%s\" has wrong major (has %li, expected %i)\n",
		       __func__, name, __FMC_MAJOR(version), FMC_MAJOR);
		return -EINVAL;
	}

	if (__FMC_MINOR(version) != FMC_MINOR)
		pr_info("%s: \"%s\" has wrong minor (has %li, expected %i)\n",
		       __func__, name, __FMC_MINOR(version), FMC_MINOR);
	return 0;
}

static int fmc_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	//struct fmc_device *fdev = to_fmc_device(dev);

	/* FIXME: The MODALIAS */
	add_uevent_var(env, "MODALIAS=%s", "fmc");
	return 0;
}

static int fmc_probe(struct device *dev)
{
	struct fmc_driver *fdrv = to_fmc_driver(dev->driver);
	struct fmc_device *fdev = to_fmc_device(dev);

	return fdrv->probe(fdev);
}

static int fmc_remove(struct device *dev)
{
	struct fmc_driver *fdrv = to_fmc_driver(dev->driver);
	struct fmc_device *fdev = to_fmc_device(dev);

	return fdrv->remove(fdev);
}

static void fmc_shutdown(struct device *dev)
{
	/* not implemented but mandatory */
}

static struct bus_type fmc_bus_type = {
	.name = "fmc",
	.match = fmc_match,
	.uevent = fmc_uevent,
	.probe = fmc_probe,
	.remove = fmc_remove,
	.shutdown = fmc_shutdown,
};

/* Every device must have a release method: provide a default */
static void __fmc_release(struct device *dev){ }

/* This is needed as parent for our devices and dir in sysfs */
struct device fmc_bus = {
	.release = __fmc_release,
	/* We can't set init_name, becase 2.6.26 and earlier miss it */
};

/*
 * Functions for client modules follow
 */

int fmc_driver_register(struct fmc_driver *drv)
{
	if (fmc_check_version(drv->version, drv->driver.name))
		return -EINVAL;
	drv->driver.bus = &fmc_bus_type;
	return driver_register(&drv->driver);
}
EXPORT_SYMBOL(fmc_driver_register);

void fmc_driver_unregister(struct fmc_driver *drv)
{
	driver_unregister(&drv->driver);
}
EXPORT_SYMBOL(fmc_driver_unregister);

/*
 * When a device set is registered, all eeproms must be read
 *and all FRU must be parsed
 */
int fmc_device_register_n(struct fmc_device *fmcs, int n)
{
	struct fmc_device *fmc, **devarray;
	uint32_t device_id;
	int i, ret = 0;

	/* Check the version of the first data structure (function prints) */
	if (fmc_check_version(fmcs->version, fmcs->carrier_name))
		return -EINVAL;

	devarray = kmalloc(n * sizeof(*devarray),GFP_KERNEL);
	if (!devarray) return -ENOMEM;

	/* Make all other checks before continuing, for all devices */
	for (i = 0, fmc = fmcs; i < n; i++, fmc++) {
		if (!fmc->hwdev) {
			pr_err("%s: device has no hwdev pointer\n", __func__);
			return -EINVAL;
		}
		if (!fmc->eeprom) {
			dev_err(fmc->hwdev, "no eeprom provided to fmc bus\n");
			ret = -EINVAL;
		}
		if (!fmc->eeprom_addr) {
			dev_err(fmc->hwdev, "eeprom_addr must be set\n");
			ret = -EINVAL;
		}
		if (!fmc->carrier_name || !fmc->carrier_data || \
		    !fmc->device_id) {
			dev_err(fmc->hwdev, "carrier name and data, and dev_id"
				"must all be set\n");
			ret = -EINVAL;
		}
		if(ret)
			break;

		fmc->nr_slots = n;
		devarray[i] = fmc;
	}
	if (ret) {
		kfree(devarray);
		return ret;
	}

	/* Validation is ok. Now init and register the devices */
	for (i = 0, fmc = fmcs; i < n; i++, fmc++) {

		fmc->devarray = devarray;

		/* make sure dev is not initialized, or it complains */
		memset(&fmc->dev.kobj, 0, sizeof(struct kobject));

		device_initialize(&fmc->dev);
		if (!fmc->dev.release)
			fmc->dev.release = __fmc_release;
		if (!fmc->dev.parent)
			fmc->dev.parent = &fmc_bus;

		/* Fill the identification stuff (may fail) */
		fmc_fill_id_info(fmc);

		fmc->dev.bus = &fmc_bus_type;

		/* Name from mezzanine info or carrier info. Or 0,1,2.. */
		device_id = fmc->device_id;
		if (!fmc->mezzanine_name) {
			dev_warn(fmc->hwdev, "No mezzanine_name found\n");
			dev_set_name(&fmc->dev, "fmc-%04x", device_id);
		} else {
			dev_set_name(&fmc->dev, "%s-%04x", fmc->mezzanine_name,
				     device_id);
		}
		ret = device_add(&fmc->dev);
		if (ret < 0) {
			dev_err(fmc->hwdev, "Failed in registering \"%s\"\n",
				dev_name(&fmc->dev));
			fmc_free_id_info(fmc);
			goto out;
		}
		/* This device went well, give information to the user */
		fmc_dump_eeprom(fmc);
		fmc_dump_sdb(fmc);
	}
	return 0;

out:
	for (i--, fmc--; i >= 0; i--, fmc--) {
		device_del(&fmc->dev);
		fmc_free_id_info(fmc);
		put_device(&fmc->dev);
	}
	kfree(fmcs->devarray);
	for (i = 0, fmc = fmcs; i < n; i++, fmc++)
		fmc->devarray = NULL;
	return ret;

}
EXPORT_SYMBOL(fmc_device_register_n);

int fmc_device_register(struct fmc_device *fmc)
{
	return fmc_device_register_n(fmc, 1);
}
EXPORT_SYMBOL(fmc_device_register);

void fmc_device_unregister_n(struct fmc_device *fmcs, int n)
{
	struct fmc_device *fmc;
	int i;

	for (i = 0, fmc = fmcs; i < n; i++, fmc++) {
		device_del(&fmc->dev);
		fmc_free_id_info(fmc);
		put_device(&fmc->dev);
	}
	/* Then, free the locally-allocated stuff */
	for (i = 0, fmc = fmcs; i < n; i++, fmc++) {
		if (i == 0)
			kfree(fmc->devarray);
		fmc->devarray = NULL;
	}
}
EXPORT_SYMBOL(fmc_device_unregister_n);

void fmc_device_unregister(struct fmc_device *fmc)
{
	fmc_device_unregister_n(fmc, 1);
}
EXPORT_SYMBOL(fmc_device_unregister);

/* Init and exit are trivial */
static int fmc_init(void)
{
	int err;

	dev_set_name(&fmc_bus, "fmc");
	err = device_register(&fmc_bus);
	if (err)
		return err;
	err = bus_register(&fmc_bus_type);
	if (err)
		device_unregister(&fmc_bus);
	return err;
}

static void fmc_exit(void)
{
	bus_unregister(&fmc_bus_type);
	device_unregister(&fmc_bus);
}

module_init(fmc_init);
module_exit(fmc_exit);

MODULE_LICENSE("GPL");
