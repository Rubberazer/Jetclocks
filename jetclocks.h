/* SPDX-License-Identifier: GPL-2.0
 * 
 * jetclocks.h - jetclocks kernel module header
 *
 * Inspired by the NVIDIA MODS kernel driver by NVIDIA CORPORATION. 
 * The jetclocks kernel module is a facility that provides a friendly API
 * for applications in user space to manipulate the clocks in Nvidia
 * Jetson Orin machines.
 *
 */

#ifndef __JETCLOCKS_H__
#define __JETCLOCKS_H__

#include <linux/ioctl.h>

struct jetclk {
    int clk_enabled;
    int clk_rate;
    char clk[20];
};

#define CLK_ENABLE		_IOW('{', 1, struct jetclk *)
#define CLK_DISABLE		_IOW('{', 2, struct jetclk *)
#define CLK_IS_ENABLED          _IOWR('{', 3, struct jetclk *)

#endif /* __JETCLOCKS_H__ */
