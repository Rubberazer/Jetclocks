/*
 * jetclock.c
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

/*static int __init jetclocks_init(void)
{
  pr_info("Init\n");
  jetclocks_probe()
  return 0;
}

static void __exit jetclocks_exit(void)
{
  pr_info("exit\n");
  }*/

static int jetclocks_probe(struct platform_device *pdev)
{
  pr_info("probe\n");
  return 0;
}

static int jetclocks_remove(struct platform_device *pdev)
{
  pr_info("remove\n");
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

/*module_init(jetclocks_init);
  module_exit(jetclocks_exit);*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rubberazer <rubberazer@outlook.com>");
MODULE_DESCRIPTION("Jetson Orin CAR for user space");
MODULE_ALIAS("platform:jetclock");
