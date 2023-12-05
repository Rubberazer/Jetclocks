/*
 *  jetgpioclk.c: Enable the Orin pwm clocks  
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
static int pwmclk_probe(void);
static int pwmclk_remove(void);


/* Enable clock function  */

static int pwmclk_probe(void){
  int ret = 0;
  
  return ret;
}

/* Disable clocks function  */

static int pwmclk_remove(void){
  int ret = 0;
  

  return ret;
}

/* This function is called when the module is loaded */

int init_module(void){
  int ret = 0;
  ret = pwmclk_probe();
  
  //pr_info("Return number probe %d.\n", ret);

  return ret;
}

/* This function is called when the module is unloaded */

void cleanup_module(void){
  
  pwmclk_remove();

  //pr_info("Return number remove %d.\n", ret);  

}

static const struct tegra_pwm_soc tegra194_pwm_soc = {
	.num_channels = 1,
	.max_frequency = 408000000UL,
};

static const struct of_device_id jetgpio_of_match[] = {
	{ .compatible = "nvidia,tegra194-pwm", .data = &tegra194_pwm_soc },
	{ }
};

MODULE_DEVICE_TABLE(of, jetgpio_of_match);

static struct platform_driver jetgpio_driver = {
	.driver = {
		.name = "jetgpio",
		.of_match_table = jetgpio_of_match,
		.pm = &tegra_pwm_pm_ops,
	},
	.probe = tegra_pwm_probe,
	.remove = tegra_pwm_remove,
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RUBBERAZER");
MODULE_DESCRIPTION("PWM clocks switch");
MODULE_INFO(intree, "Y");
