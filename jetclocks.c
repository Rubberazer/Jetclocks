/* SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * jetclocks.c
 *
 * Based on the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
 * The jetclocks kernel module is a facility that provides a friendly API
 * to applications in user space to the clocks and resets (CAR) in 
 * Nvidia Jetson Orin machines.
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
    struct reset_control*rst;
};

static int jetclocks_probe(struct platform_device *pdev)
{
    struct  jetclocks *jetclock_n;
    
    int ret;
    
    pr_info("starting probe\n");
    
    jetclock_n = devm_kzalloc(&pdev->dev, sizeof(*jetclock_n), GFP_KERNEL);
    if (!jetclock_n)
	return -ENOMEM;
    
    jetclock_n->dev = &pdev->dev;
    platform_set_drvdata(pdev, jetclock_n);
    
    jetclock_n->clk = devm_clk_get(&pdev->dev, "pwm1");
    if (IS_ERR(jetclock_n->clk))
	return PTR_ERR(jetclock_n->clk);

    ret = clk_prepare(jetclock_n->clk);
    if (ret) {
	dev_err(&pdev->dev, "Clock prepare failed\n");
	return ret;
    }

    ret = clk_enable(jetclock_n->clk);
    if (ret) {
	dev_err(&pdev->dev, "Clock prepare failed\n");
	return ret;
    }
    return 0;
}

static int jetclocks_remove(struct platform_device *pdev)
{
    struct jetclocks *jetclock_r;

    pr_info("starting remove\n");

    jetclock_r = platform_get_drvdata(pdev);

    if (WARN_ON(!jetclock_r))
	return -ENODEV;

    clk_disable_unprepare(jetclock_r->clk);
    return 0;
}

static const struct of_device_id jetclocks_of_match[] = {
    { .compatible = "nvidia,mods-clocks"},
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

