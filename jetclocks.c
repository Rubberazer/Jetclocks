/* SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * jetclocks.c
 *
 * Inspired by the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
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
    struct cdev cdev;
    struct device *dev;
    struct clk *clk;
    struct reset_control *rst;
};

static unsigned int major;
static dev_t major_devt;
static struct jetclocks *jetclocks_dev;
static struct class *cls;

int jetclocks_open(struct inode * inode, struct file * filp)
{
    pr_info("Opening jetclocks chardev\n");
    return 0;
}

int jetclocks_release(struct inode * inode, struct file * filp)
{
    pr_info("Closing jetclocks chardev\n");
    return 0;
}
/*
static long jetclocks_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    switch(cmd){
    case CLK_ENABLE:
	clock_enable();
	break;
    case CLK_DISABLE:
	clock_disable();
	break;
    case CLK_IS_ENABLED:
	clock_is_enable();
	break;
    default:
	return -ENOTTY;
    }
    return 0;
} */

struct file_operations jetclocks_fops = {
    owner:          THIS_MODULE,
    open:           jetclocks_open,
    release:        jetclocks_release,
    //unlocked_ioctl: jetclocks_ioctl,
};

static int is_clock_enabled(const char *clock, struct platform_device *pdev)
{
    struct jetclocks *jetclock_i;
       
    jetclock_i = platform_get_drvdata(pdev);
    if (WARN_ON(!jetclock_i))
	 return -ENODEV;
    
    jetclock_i->clk = devm_clk_get(&pdev->dev, clock);
    if (IS_ERR(jetclock_i->clk))
	return PTR_ERR(jetclock_i->clk);

    if (!jetclock_i->clk) {
	return 0;
    }
    
    return 1;
}

static int clock_enable(const char *clock, struct platform_device *pdev)
{
    struct jetclocks *jetclock_e;
    int ret;
       
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
    int ret = 0;
    
    pr_info("Probing jetclocks\n");
    
    jetclocks_dev = devm_kzalloc(&pdev->dev, sizeof(*jetclocks_dev), GFP_KERNEL);
    if (!jetclocks_dev)
	return -ENOMEM;
    
    jetclocks_dev->dev = &pdev->dev;
    platform_set_drvdata(pdev, jetclocks_dev);

    /* Character device */

    if (alloc_chrdev_region(&major_devt, 0, 1, "jetclocks") < 0) {
	pr_err("Device number could not be allocated!\n");
	return -EINVAL;
    }

    major = MAJOR(major_devt);
    
    if ((cls = class_create(THIS_MODULE, "jetclockclass")) == NULL) {
	pr_err("Device class can not be created!\n");
	class_destroy(cls);
	return -EINVAL;
    } 
    
    if (device_create(cls, NULL, major_devt, NULL, "jetclocks") == NULL) {
	printk("Can not create device file!\n");
	unregister_chrdev_region(major_devt, 1);
	return -ENODEV;
    }
       
    cdev_init(&jetclocks_dev->cdev, &jetclocks_fops);
    
    if (cdev_add(&jetclocks_dev->cdev, major_devt, 1) == -1) {
	printk("Registering of device to kernel failed!\n");
	device_destroy(cls, major_devt);
	return -ENODEV;
    }
        
    pr_info("jetclocks module loaded\n");
   
    return ret;
}

static int jetclocks_remove(struct platform_device *pdev)
{
    dev_t major_devt = MKDEV(major, 0);
    int ret = 0;
    
    pr_info("Removing jetclocks\n");
    
    cdev_del(&jetclocks_dev->cdev);
    device_destroy(cls, major_devt);
    class_destroy(cls);
    unregister_chrdev_region(major_devt, 1);
    
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

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Rubberazer <rubberazer@outlook.com>");
MODULE_DESCRIPTION("Jetson Orin CAR for user space");
