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
#include <linux/slab.h>

struct jetclocks {
    struct device *dev;
    struct clk *clk;
    struct reset_control *rst;
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
    const char *clock = "pwm1";
    
    int ret = 0;
    
    pr_info("Probing jetclocks\n");
    
    jetclock_p = devm_kzalloc(&pdev->dev, sizeof(*jetclock_p), GFP_KERNEL);
    if (!jetclock_p)
	return -ENOMEM;
    
    jetclock_p->dev = &pdev->dev;
    platform_set_drvdata(pdev, jetclock_p);

    ret = clock_enable(clock, pdev);
   
    return ret;
}

static int jetclocks_remove(struct platform_device *pdev)
{
    struct jetclocks *jetclock_r;
    int ret = 0;
    const char *clock = "pwm1";
    
    pr_info("Removing jetclocks\n");

    jetclock_r = platform_get_drvdata(pdev);
    if (WARN_ON(!jetclock_r))
	 return -ENODEV;
    
    ret = clock_disable(clock, pdev);
    
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
MODULE_ALIAS("platform:jetclock");

