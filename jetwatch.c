/* GNU GENERAL PUBLIC LICENSE Version 2
 *  
 * jetwatch.c: Kernel loadable module to manage the Orin clocks
 *  
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk.h>


/* Global variables */

/* Prototypes*/

int init_module(void);
void cleanup_module(void);
static int jetwatch_probe(void);
static int jetwatch_remove(void);

/* Enable clock  */

static int jetwatch_probe(void){
  int ret = 0;
  
  return ret;
}

/* Disable clock  */

static int jetwatch_remove(void){
  int ret = 0;
  
  return ret;
}

/* This function is called when the module is loaded */

int init_module(void){
  int ret = 0;
  ret = jetwatch_probe();
  
  //pr_info("Return number probe %d.\n", ret);

  return ret;
}

/* This function is called when the module is unloaded */

void cleanup_module(void){
  
  jetwatch_remove();

  //pr_info("Return number remove %d.\n", ret);  

}

static const struct of_device_id jetwatch_of_match[] = {
	{ .compatible = "nvidia,tegra194-pwm", .data = &tegra194_pwm_soc },
	{ }
};

MODULE_DEVICE_TABLE(of, jetgpio_of_match);

static struct platform_driver jetwatch_driver = {
	.driver = {
		.name = "jetwatch",
		.of_match_table = jetwatch_of_match,
		.pm = &tegra_pwm_pm_ops,
	},
	.probe = jetwatch_probe,
	.remove = jetwatch_remove,
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RUBBERAZER");
MODULE_DESCRIPTION("Orin clock manager");
MODULE_INFO(intree, "Y");
