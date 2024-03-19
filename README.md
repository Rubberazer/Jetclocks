# Jetclocks
Simple kernel module to control the Jetson Orin clocks.
The aim of this kernel module is to provide applications in user space access to the clocks in Jetson Orin machines through system calls - ioctls(). Basically is a wrapper of the standard Linux Kernel Common Clock Framework. 

This module is inspired by the NVIDIA MODS kernel driver by NVIDIA CORPORATION. For more information on the MODS kernel driver check the Linux for Tegra kernel sources.

<h2 align="left">SUPPORTED MODELS:</h2>

- Jetson Orin family: Orin Nano, NX and AGX

<h2 align="left">FUNCTIONALITY:</h2>

- Enable/disable any clock
- Check whether a clock is enabled or disabled

<h2 align="left">DEVICE TREE:</h2>

This module is basically a platfrom driver and makes use of the device tree, so the first step is to compile the device tree overlay and use it to patch the main device tree blob, there are several ways of going about this, the following one has been tested and it doesn't imply reflashing or recompiling the whole kernel.
DISCLAIMER - Manipulating the device tree blob might cause your system to behave in unexpected, undesirable ways, handle with care.

- First of all, clone/download this module sources anywhere in your Orin, say to your home directory:

  	cd ~/
	git clone https://github.com/Rubberazer/Jetclocks.git

- Download the Linux for Tegra kernel sources according the version of the kernel running on your Orin, all versions can be downloaded directly from: https://developer.nvidia.com/embedded/jetson-linux-archive

- Unzip de sources, for instance on your /home directory, you should get a folder looking like ~/Linux_for_Tegra/ with all the kernel sources inside

- Preprocess the overlay file with some kernel headers:

    ```
    cd ~/Jetclocks
    cpp -nostdinc -I ~/Linux_for_Tegra/source/public/hardware/nvidia/soc/t23x/kernel-include/ -undef -x assembler-with-cpp jetclocks.dtsi  jetclocks.dtsi.new

    ```

- Compile overlay:

    ```
    dtc -I dts -O dtb ./jetclocks.dtsi.new -o ./jetclocks.dtbo

    ```

- Check current device tree in use:

    ```
    sudo nano /boot/extlinux/extlinux.conf
    ```

- You should have something like:

  TIMEOUT 30
  DEFAULT primary

  MENU TITLE L4T boot options

  LABEL primary

  LINUX /boot/Image
  FDT /boot/dtb/kernel_tegra234-p3767-0004-p3768-0000-a0.dtb

  ...

- Make sure there is a backup entry, copy and paste the primary label inmediately below, call it backup, afterwards rename the FDT with the name of the future device tree blob on the primary one, let's call it new in our example. It should look something like this:

  TIMEOUT 30
  DEFAULT primary

  MENU TITLE L4T boot options

  LABEL primary

  LINUX /boot/Image
  FDT /boot/dtb/new.dtb

  ...

  LABEL backup

  LINUX /boot/Image
  FDT /boot/dtb/kernel_tegra234-p3767-0004-p3768-0000-a0.dtb

  ...
	
- Apply overlay blob (dtbo) to main dt blob

    ```
    cd ~/Jetclocks
    sudo cp jetclocks.dtbo /boot/dtb
    cd /boot/dtb
    sudo fdtoverlay -i kernel_tegra234-p3767-0004-p3768-0000-a0.dtb -o new.dtb jetclocks.dtbo

    ```

- Make sure that everything is OK, if you corrupt your device tree and your backup label in extlinux.conf is not correct you will be in trouble.

- Reboot, after reboot you can check wheter the overlay has been applied:

    ```
    ls /proc/device-tree/jetclocks/

    ```
    If everything went OK, you should see:
    ```
    clock-names  clocks  compatible  name  status

    ```
    
<h2 align="left">COMPILE THE MODULE:</h2>

To compile the module just:


    make


<h2 align="left">DEPLOYMENT:</h2>

To insert the module:


    sudo insmod jetclocks.ko


If you type a:

    
    sudo dmesg

   
You should see something like below, you can ignore the "tainting kernel" message, this just means that is not signed (out of tree module), it doesn't have any practical implications:

    
    Probing jetclocks
    jetclocks module loaded

    
To remove the module:

   
    sudo rmmod jetclocks

    
<h2 align="left">USE THE MODULE:</h2>

This module is all about using it from user space, there are 3 basic operations that you can perform from your user space applications that interact with this module. You are going to need a list with valid clock names, you can find this at [clock_names.md](https://github.com/Rubberazer/Jetclocks/blob/main/clock_names.md). There is also a sample application here [jetclocks_user](https://github.com/Rubberazer/Jetclocks/blob/main/jetclocks_user.c).
LIMITATIONS: you are not going to be able to enable or disable clocks already enabled (in use by some other peripheral), this is due to the fact that is not going ot be able to get the clock handler. So basically, if some clock is already disabled you will e bale to enable and disable it from that point, but not if it is already in use.

Check whether a clock is enabled:
```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

struct jetclk clock = {0};

int main() {

    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev == -1) {
	printf("Opening /dev/jetclocks not possible\n");
	return -1;
    }

    /* Now checking whether the clok is enabled*/

    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    ioctl(dev, CLK_IS_ENABLED, &clock);
    printf("clock %s status(0 disabled, 1 enabled): %d\n",clock.clk, clock.clk_enabled);
 
    close(dev);
    return 0;
}
```
Enable clock:

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

struct jetclk clock = {0};

int main() {

    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev == -1) {
	printf("Opening /dev/jetclocks not possible\n");
	return -1;
    }

    /* Enabling clock "spi1" */
    
    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    ioctl(dev, CLK_ENABLE, &clock);
    printf("Enabling clock %s: \n",clock.clk);

    close(dev);
    return 0;
}
```

Disable clock:

```
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "jetclocks.h"

struct jetclk clock = {0};

int main() {

    int dev = open("/dev/jetclocks", O_WRONLY);
    if(dev == -1) {
	printf("Opening /dev/jetclocks not possible\n");
	return -1;
    }

    /* Disabling clock "spi1" */
    
    strncpy(clock.clk, "spi1", sizeof(clock.clk));
    ioctl(dev, CLK_DISABLE, &clock);
    printf("Disabling clock %s: \n",clock.clk);

    close(dev);
    return 0;
}
```
