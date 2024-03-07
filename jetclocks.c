/* SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * jetclocks.c
 *
 * Based on the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
 * The jetclocks kernel module is a facility that provides a friendly API
 * for applications in user space to access the Clocks and Resets Controller 
 * (CAR) in Nvidia Jetson Orin machines.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/slab.h>

struct jetclocks {
    struct device *dev;
    struct clk *clk;
    struct reset_control *rst;
};

static unsigned int major;
static struct cdev jetclocks_dev;
static dev_t major_devt;
static struct class *cls;

int jetclocks_open(struct inode * inode, struct file * filp)
{
    pr_info("Someone tried to open me\n");
    return 0;
}

int jetclocks_release(struct inode * inode, struct file * filp)
{
    pr_info("Someone closed me\n");
    return 0;
}

ssize_t jetclocks_read (struct file *filp, char __user * buf, size_t count,
                                loff_t * offset)
{
    pr_info("Nothing to read guy\n");
    return 0;
}


ssize_t jetclocks_write(struct file * filp, const char __user * buf, size_t count,
                                loff_t * offset)
{
    pr_info("Can't accept any data guy\n");
    return count;
}

struct file_operations jetclocks_fops = {
    owner:      THIS_MODULE,
    open:       jetclocks_open,
    release:    jetclocks_release,
    read:       jetclocks_read,
    write:      jetclocks_write,
};

static int clock_enable(const char *clock, struct platform_device *pdev)
{
    int ret;
    
    struct jetclocks *jetclock_e;
    
    jetclock_e = platform_get_drvdata(pdev);
    if (WARN_ON(!jetclock_e))
	 return -ENODEV;
    
    jetclock_e->clk = devm_clk_get(&pdev->dev, clock);
    if (IS_ERR(jetclock_e->clk))
	return PTR_ERR(jetclock_e->clk);

    ret = clk_prepare(jetclock_e->clk);
    if (ret) {
	dev_err(&pdev->dev, "Clock prepare failed\n");
	return ret;
    }

    ret = clk_enable(jetclock_e->clk);
    if (ret) {
	dev_err(&pdev->dev, "Clock enable failed\n");
	return ret;
    }

    return 0;
}

static int clock_disable(const char *clock, struct platform_device *pdev)
{
    struct  jetclocks *jetclock_d;

    jetclock_d = platform_get_drvdata(pdev);
    if (WARN_ON(!jetclock_d))
	 return -ENODEV;

    jetclock_d->clk = devm_clk_get(&pdev->dev, clock);
    if (IS_ERR(jetclock_d->clk))
	return PTR_ERR(jetclock_d->clk);

    clk_disable_unprepare(jetclock_d->clk);

    return 0;
}

static int jetclocks_probe(struct platform_device *pdev)
{
    struct  jetclocks *jetclock_p;
    int ret = 0;
    
    pr_info("Probing jetclocks\n");
    
    jetclock_p = devm_kzalloc(&pdev->dev, sizeof(*jetclock_p), GFP_KERNEL);
    if (!jetclock_p)
	return -ENOMEM;
    
    jetclock_p->dev = &pdev->dev;
    platform_set_drvdata(pdev, jetclock_p);

    /* Character device */

    if (alloc_chrdev_region(&major_devt, 0, 1, "jetclk") < 0) {
	pr_err("Device number could not be allocated!\n");
	return -1;
    }

    if ((cls = class_create(THIS_MODULE, "jetclkclass")) == NULL) {
	pr_err("Device class can not be created!\n");
	unregister_chrdev_region(major_devt, 1);
	return -1;
    } 

    
    if (device_create(cls, NULL, major_devt, NULL, "jetclk") == NULL) {
	printk("Can not create device file!\n");
	class_destroy(cls);
	return -1;
    }
    
	/* Initialize device file */
    cdev_init(&jetclocks_dev, &jetclocks_fops);
    
    /* Register device to kernel */
    if (cdev_add(&jetclocks_dev, major_devt, 1) == -1) {
	printk("Registering of device to kernel failed!\n");
	device_destroy(cls, major_devt);
	return -1;
    }
        
    pr_info("jetclocks module loaded\n");
   
    return ret;
}

static int jetclocks_remove(struct platform_device *pdev)
{
    struct jetclocks *jetclock_r;
    int ret = 0;
    
    pr_info("Removing jetclocks\n");

    jetclock_r = platform_get_drvdata(pdev);
    if (WARN_ON(!jetclock_r))
	 return -ENODEV;

    dev_t dev = MKDEV(major, 0);

    cdev_del(&jetclocks_dev);
    unregister_chrdev_region(dev, 1);
    
    pr_info("jetclocks module unloaded\n");
    
    return ret;
}

static const struct of_device_id jetclocks_of_match[] = {
    { .compatible = "nvidia,jetclocks"},
    { }
};

MODULE_DEVICE_TABLE(of, jetclocks_of_match);

static struct platform_driver jetclocks_driver = {
    .driver = {
        .name = "jetclocks",
        .of_match_table = jetclocks_of_match,
    },
    .probe = jetclocks_probe,
    .remove = jetclocks_remove,
};

module_platform_driver(jetclocks_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rubberazer <rubberazer@outlook.com>");
MODULE_DESCRIPTION("Jetson Orin CAR for user space");
