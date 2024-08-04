SHELL:=/bin/bash
obj-m += jetclocks.o

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
	@touch /etc/modules-load.d/jetclocks.conf
	@echo "# Load jetclocks module\njetclocks" > /etc/modules-load.d/jetclocks.conf
	depmod -a
	@echo "jetclocks.conf created into /etc/modules-load.d/"
	@echo "Finding device tree blob"
	$(eval DTBFILE := $(shell find /boot/dtb -name "*kernel*.dtb"  -exec basename {} \;))
	@echo "Creating backup of: /boot/dtb/$(DTBFILE) -> /boot/dtb/$(DTBFILE).jetclocks.backup"
	@cp /boot/dtb/$(DTBFILE) /boot/dtb/$(DTBFILE).jetclocks.backup
	@echo "Applying overlay blob to: /boot/dtb/$(DTBFILE)"
	@fdtoverlay -i /boot/dtb/$(DTBFILE) -o /boot/dtb/jetclocks.dtb $(PWD)/overlay_blob/jetclocks.dtbo
	$(eval EXTFILE := $(shell find /boot/extlinux -name "extlinux.conf" -exec basename {} \;))
	$(eval CHECKER := $(shell grep /boot/extlinux/extlinux.conf -e "DEFAULT jetclocks"))
	@if [ "$(CHECKER)" = "" ]; then\
		echo "Creating backup of: /boot/extlinux/$(EXTFILE) -> /boot/extlinux/$(EXTFILE).jetclocks.backup";\
		cp /boot/extlinux/$(EXTFILE) /boot/extlinux/$(EXTFILE).jetclocks.backup;\
		echo "Modifying: /boot/extlinux/$(EXTFILE)";\
		echo "`awk '/LABEL primary/{f=1} /APPEND/{f=0;print} f' /boot/extlinux/extlinux.conf | sed '/#/d'`" > $(PWD)/extlinux.conf.temp;\
		sed -i 's/LABEL\sprimary/LABEL jetclocks/g' $(PWD)/extlinux.conf.temp;\
		sed -i '/INITRD/a\      FDT /boot/dtb/jetclocks.dtb' $(PWD)/extlinux.conf.temp;\
		echo "`cat $(PWD)/extlinux.conf.temp`" >> /boot/extlinux/$(EXTFILE);\
		sed -i 's/DEFAULT\sprimary/DEFAULT jetclocks/g' /boot/extlinux/$(EXTFILE);\
		rm -f $(PWD)/extlinux.conf.temp;\
	fi
	@echo "All done, you should reboot the machine now"

uninstall:
	$(eval JETFILE := $(shell find /lib/modules/$(uname -r) -name "jetclocks.ko"))
	@rm -f $(JETFILE)
	@rm -f /etc/modules-load.d/jetclocks.conf
	depmod -a
	@echo "$(JETFILE) removed"	
	@echo "/etc/modules-load.d/jetclocks.conf removed"
