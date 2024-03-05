## Extract dts
dtc -o - -O dts -I dtb device.dtb

# Convert dtb to dts
dtc -I dtb -O dts kernel_tegra234-p3767-0004-p3768-0000-a0.dtb -o /tmp/tmp.dts

# Convert dts to dtb
dtc -I dts -O dtb /tmp/tmp.dts -o /tmp/tmp.dtb

## Overlays - preprocess overlay with includes
cpp -nostdinc -I ~/development/Linux_for_Tegra/source/public/hardware/nvidia/soc/t23x/kernel-include/ -undef -x assembler-with-cpp jetclocks.dtsi  jetclocks.dtsi.prep

# Compile overlay
dtc -I dts -O dtb ./jetclocks.dtsi.prep -o ./jetclocks.dtbo

# Apply overlay blob (dtbo) to main dt blob

fdtoverlay -i tegra210-p3448-0002-p3449-0000-b00.dtb -o tegra210-p3448-0002-p3449-0000-b00-extended.dtb my-overlay.dtbo

## dtb blob in use
/boot/kernel_tegra234-p3767-0004-p3768-0000-a0-user-custom.dtb

## Original dtb (before jetson-io.py)
/boot/dtb/kernel_tegra234-p3767-0004-p3768-0000-a0.dtb


