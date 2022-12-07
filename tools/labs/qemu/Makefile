.PHONY: qemu clean

QEMU_DISPLAY ?= none
ARCH ?= x86
ifeq ($(ARCH),x86)
b = b
endif

ZIMAGE = $(KDIR)/arch/$(ARCH)/boot/$(b)zImage
KCONFIG = $(KDIR)/.config
NTTCP = $(KDIR)/tools/labs/templates/assignments/6-e100/nttcp

#YOCTO_URL = http://downloads.yoctoproject.org/releases/yocto/yocto-2.3/machines/qemu/qemu$(ARCH)/
YOCTO_URL = https://mirror.fit.cvut.cz/NI-OSY/
#YOCTO_IMAGE = core-image-minimal-qemu$(ARCH).ext4
#YOCTO_IMAGE = core-image-minimal-dev-qemu$(ARCH).ext4
#YOCTO_IMAGE = core-image-sato-dev-qemu$(ARCH).ext4
YOCTO_IMAGE = core-image-sato-qemu$(ARCH).ext4
#YOCTO_IMAGE = core-image-sato-sdk-qemu$(ARCH).ext4

QEMU_OPTS = -kernel $(ZIMAGE) \
	-device virtio-serial \
	-chardev pty,id=virtiocon0 -device virtconsole,chardev=virtiocon0 \
	-serial pipe:pipe1 -serial pipe:pipe2 \
	-netdev tap,id=tap0,ifname=tap0,script=no,downscript=no -net nic,netdev=tap0,model=virtio \
	-netdev tap,id=tap1,ifname=tap1,script=no,downscript=no -net nic,netdev=tap1,model=i82559er \
	-drive file=$(YOCTO_IMAGE),if=virtio,format=raw \
	-drive file=disk1.img,if=virtio,format=raw \
	-drive file=disk2.img,if=virtio,format=raw \
	--append "root=/dev/vda loglevel=15 console=hvc0" \
	--display $(QEMU_DISPLAY) -s -m 256

ifdef ENABLE_KVM
QEMU_OPTS += -enable-kvm -cpu host
ENABLE_KVM_SUDO = sudo
endif

# Run with:
# make -j$(nproc)

# Compile in skel directories with something like:
# alias kmake='make -C "$HOME/src/linux/" M="$(pwd)"'
# kmake

# .ssh/config
# Host osy
#     HostName localhost
#     Port 2222
#     User root
#     HostkeyAlgorithms +ssh-rsa
#     PubkeyAcceptedAlgorithms +ssh-rsa

# Mount SMB (if automount fails)
# mount -a
#  or if customization is needed:
# mount -t cifs -o guest,user=guest //10.0.2.4/qemu skels

# For GUI change:
#		-append "root=/dev/vda console=ttyS0 loglevel=15 TERM=xterm" \
#		-nographic \
# to:
#		-append "root=/dev/vda loglevel=15" \
#		-display sdl \

qemu: $(ZIMAGE) $(YOCTO_IMAGE)
	qemu-system-i386 \
		-enable-kvm -cpu host \
		-smp 2 -m 2048 \
		-no-reboot \
		-nographic \
		-drive file=$(YOCTO_IMAGE),if=virtio,format=raw \
		-nic user,hostfwd=tcp::2222-:22,smb=$(shell readlink -f skels) \
		-kernel $(ZIMAGE) \
		-append "root=/dev/vda console=ttyS0 loglevel=15 TERM=xterm" \
		-nographic \
		-gdb tcp::1234

boot: .modinst tap0 tap1 pipe1.in pipe1.out pipe2.in pipe2.out disk1.img disk2.img nttcp-run
	$(ENABLE_KVM_SUDO) ARCH=$(ARCH) qemu/qemu.sh $(QEMU_OPTS)

TEMPDIR := $(shell mktemp -u)

$(KCONFIG): qemu/kernel_config.x86
	cp $^ $@
	$(MAKE) -C $(KDIR) olddefconfig

zImage: $(ZIMAGE)

$(ZIMAGE): $(KCONFIG)
	$(MAKE) -C $(KDIR)
	$(MAKE) -C $(KDIR) modules

.modinst: $(ZIMAGE) $(YOCTO_IMAGE)
	mkdir $(TEMPDIR)
	sudo mount -t ext4 -o loop $(YOCTO_IMAGE) $(TEMPDIR)
	sudo $(MAKE) -C $(KDIR) modules_install INSTALL_MOD_PATH=$(TEMPDIR)
	sudo umount $(TEMPDIR)
	rmdir $(TEMPDIR)
	sleep 1 && touch .modinst

gdb: $(ZIMAGE)
	gdb -ex "target remote localhost:1234" $(KDIR)/vmlinux

$(YOCTO_IMAGE):
	wget $(YOCTO_URL)/$(YOCTO_IMAGE)
	sudo ARCH=$(ARCH) qemu/prepare-image.sh $(YOCTO_IMAGE)

tap0:
	qemu/create_net.sh $@

tap1:
	qemu/create_net.sh $@

nttcp-run: $(NTTCP) tap1
	$(NTTCP) -v -i &

pipe1.in:
	mkfifo $@

pipe1.out:
	mkfifo $@

pipe2.in: pipe1.out
	ln $< $@

pipe2.out: pipe1.in
	ln $< $@

disk1.img:
	qemu-img create -f raw $@ 100M

disk2.img:
	qemu-img create -f raw $@ 100M

clean::
	-rm -f .modinst
	-rm -f disk1.img disk2.img
	-rm -f pipe1.in pipe1.out pipe2.in pipe2.out

.PHONY: boot gdb clean tap0 tap1
