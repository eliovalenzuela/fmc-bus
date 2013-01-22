/* Fixes for 2.6.24 */
#include <linux/version.h>

/* to_delayed_work() appeared in 2.6.30-rc1 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
static inline struct delayed_work *to_delayed_work(struct work_struct *work)
{
        return container_of(work, struct delayed_work, work);
}
#endif

/* dev_name() appeared in v2.6.26-rc1 */
/* dev_set_name() appeared in v2.6.26-rc6 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
static inline const char *dev_name(struct device *dev)
{
        return kobject_name(&dev->kobj);
}

#define dev_set_name(dev, fmt, ...)  ({ \
        int err = 0; \
        err = kobject_set_name(&(dev)->kobj, fmt, ## __VA_ARGS__); \
        if (!err) \
                strncpy((dev)->bus_id, (dev)->kobj.k_name, BUS_ID_SIZE); \
        err; })
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
#define get_unaligned_le16(x) get_unaligned(x)
#endif
