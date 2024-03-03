# Jetsonclocks
Kernel module to control the Jetson Orin clocks.
The aim of this kernel module is to provide applications in user space access to the Cloks and Resets in Jetson Orin machines through system calls - ioctls(). This effectively provides a friendly API to the Clock and Reset Controller (CAR), for Orins this is firmware running on the BPMP controller.


This module is inspired by the NVIDIA MODS kernel driver and it uses its device tree description. See also the following files in the Linux for Tegra kernel sources:

 - /Linux_for_Tegra/source/public/kernel/nvidia/drivers/misc/mods/mods_clock.c
 - /Linux_for_Tegra/source/public/kernel/nvidia/drivers/misc/mods/mods_internal.h
 - /Linux_for_Tegra/source/public/kernel/nvidia/drivers/misc/mods/mods_krnl.c
