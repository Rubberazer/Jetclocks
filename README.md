# Jetclocks
Kernel module to control the Jetson Orin clocks.
The aim of this kernel module is to provide applications in user space access to the clocks in Jetson Orin machines through system calls - ioctls(). Basically is a wrapper of the standard Linux Kernel Common Clock Framework. 

This module is inspired by the NVIDIA MODS kernel driver by NVIDIA CORPORATION. For more information on the MODS kernel driver check the Linux for Tegra kernel sources.

<h2 align="left">SUPPORTED MODELS:</h2>

- Jetson Orin family: Orin, Nano, NX and AGX

<h2 align="left">FUNCTIONALITY:</h2>

- Enable/disable any clock
- Check whether a clock is enabled or disabled
- Set clock rate on any clock
- Get any clock's curretn rate

<h2 align="left">DEVICE TREE:</h2>

This module is basically a platfrom driver and makes use of the device tree, so the first step is to compile the device tree overlay and use it to patch the main device tree blob, there are several ways of going about this, the following one has been tested and it doesn't imply reflashing or recompiling the whole kernel.
DISCLAIMER - Manipulating the device tree blob might cause your system to not start and behave in unexpected, undesirable ways, handle with care.

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

    clock-names  clocks  compatible  name  status

<h2 align="left">COMPILE THE MODULE:</h2>

