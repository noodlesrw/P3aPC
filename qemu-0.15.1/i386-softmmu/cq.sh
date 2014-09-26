#!/bin/bash

toolpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
toolpath+=:/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib

xprefix=arm-linux-androideabi

#usrpath=/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr
#crtpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.4.3
#usrpath=/home/nathan/android/gcc4x/arm-linux-elf
#crtpath=/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0

PATH=$toolpath:$PATH \
$xprefix-gcc \
-D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
-Wstrict-prototypes -Wredundant-decls -Wall -Wundef -Wwrite-strings \
-Wmissing-prototypes -fno-strict-aliasing -I/home/nathan/android/SDL-1.2.14/include  \
-fstack-protector-all -Wendif-labels -Wmissing-include-dirs -Wempty-body \
-Wnested-externs -Wformat-security -Wformat-y2k -Winit-self -Wignored-qualifiers \
-Wold-style-declaration -Wold-style-definition -Wtype-limits \
-DHAS_AUDIO -DHAS_AUDIO_CHOICE   -DTARGET_PHYS_ADDR_BITS=64 \
-mandroid --no-exceptions -nostdlib \
-I/home/nathan/android/SDL-1.2.14/include \
-I/home/nathan/android/gcc4x/arm-linux-elf/include/SDL \
-I/home/nathan/android/gcc4x/arm-linux-elf/include \
-I../linux-headers -I.. -I/home/nathan/android/qemu-0.15.1/target-i386 -DNEED_CPU_H  \
-I/usr/arm-unknown-linux-gnueabi/usr/include/glib-2.0 \
-I/usr/arm-unknown-linux-gnueabi/usr/lib/glib-2.0/include \
arch_init.c cpus.c monitor.c machine.c gdbstub.c balloon.c virtio.c virtio-blk.c \
virtio-balloon.c virtio-net.c virtio-serial-bus.c vhost_net.c rwhandler.c \
kvm-stub.c xen-stub.c ../cutils.c ../cache-utils.c ../qemu-malloc.c \
../qemu-option.c ../module.c ../async.c ../nbd.c ../block.c ../aio.c ../aes.c \
../qemu-config.c ../qemu-progress.c ../qemu-sockets.c ../posix-aio-compat.c \
../block/raw.c ../block/cow.c ../block/qcow.c ../block/vdi.c ../block/vmdk.c \
../block/cloop.c ../block/dmg.c ../block/bochs.c ../block/vpc.c ../block/vvfat.c \
../block/qcow2.c ../block/qcow2-refcount.c ../block/qcow2-cluster.c \
../block/qcow2-snapshot.c ../block/qcow2-cache.c ../block/qed.c \
../block/qed-gencb.c ../block/qed-l2-cache.c ../block/qed-table.c \
../block/qed-cluster.c ../block/qed-check.c ../block/parallels.c ../block/nbd.c \
../block/blkdebug.c ../block/sheepdog.c ../block/blkverify.c ../block/raw-posix.c .\
./blockdev.c ../net.c ../net/queue.c ../net/checksum.c ../net/util.c \
../net/socket.c ../net/dump.c ../net/tap.c ../net/tap-linux.c ../qint.c \
../qstring.c ../qdict.c ../qlist.c ../qfloat.c ../qbool.c ../qjson.c \
../json-lexer.c ../json-streamer.c ../json-parser.c ../qerror.c ../error.c \
../readline.c ../console.c ../cursor.c ../qemu-error.c ../osdep.c \
../oslib-posix.c ../qemu-thread-posix.c ../os-posix.c ../tcg-runtime.c \
../host-utils.c ../irq.c ../ioport.c ../input.c ../i2c.c ../smbus.c \
../smbus_eeprom.c ../eeprom93xx.c ../scsi-disk.c ../cdrom.c \
../scsi-generic.c ../scsi-bus.c ../usb.c ../usb-hub.c ../usb-linux.c \
../usb-hid.c ../usb-msd.c ../usb-wacom.c ../usb-serial.c ../usb-net.c \
../usb-bus.c ../usb-desc.c ../bt.c ../bt-host.c ../bt-vhci.c ../bt-l2cap.c \
../bt-sdp.c ../bt-hci.c ../bt-hid.c ../usb-bt.c ../bt-hci-csr.c \
../buffered_file.c ../migration.c ../migration-tcp.c ../qemu-char.c \
../savevm.c ../msmouse.c ../ps2.c ../qdev.c ../qdev-properties.c \
../block-migration.c ../iohandler.c ../pflib.c ../bitmap.c ../bitops.c \
../migration-exec.c ../migration-unix.c ../migration-fd.c \
../audio/audio.c ../audio/noaudio.c ../audio/wavaudio.c ../audio/mixeng.c \
../audio/sdlaudio.c ../audio/wavcapture.c ../ui/keymaps.c \
../ui/sdl.c ../ui/sdl_zoom.c ../ui/x_keymap.c ../iov.c ../acl.c \
../compatfd.c ../notify.c ../event_notifier.c ../qemu-timer.c \
../qemu-timer-common.c ../libdis/i386-dis.c exec.c translate-all.c \
cpu-exec.c translate.c tcg/tcg.c fpu/softfloat.c op_helper.c helper.c \
cpuid.c disas.c ../libhw64/vl.c ../libhw64/loader.c ../libhw64/virtio-console.c \
../libhw64/virtio-pci.c ../libhw64/fw_cfg.c ../libhw64/pci.c \
../libhw64/pci_bridge.c ../libhw64/msix.c ../libhw64/msi.c ../libhw64/pci_host.c \
../libhw64/pcie_host.c ../libhw64/ioh3420.c ../libhw64/xio3130_upstream.c \
../libhw64/xio3130_downstream.c ../libhw64/watchdog.c ../libhw64/serial.c \
../libhw64/parallel.c ../libhw64/i8254.c ../libhw64/pcspk.c ../libhw64/pckbd.c \
../libhw64/usb-uhci.c ../libhw64/usb-ohci.c ../libhw64/usb-ehci.c ../libhw64/fdc.c \
../libhw64/acpi.c ../libhw64/acpi_piix4.c ../libhw64/pm_smbus.c ../libhw64/apm.c \
../libhw64/dma.c ../libhw64/hpet.c ../libhw64/applesmc.c ../libhw64/wdt_i6300esb.c \
../libhw64/pcie.c ../libhw64/pcie_aer.c ../libhw64/pcie_port.c ../libhw64/ne2000.c \
../libhw64/eepro100.c ../libhw64/pcnet-pci.c ../libhw64/pcnet.c ../libhw64/e1000.c \
../libhw64/rtl8139.c ../libhw64/ne2000-isa.c ../libhw64/ide/core.c \
../libhw64/ide/atapi.c ../libhw64/ide/qdev.c ../libhw64/ide/pci.c \
../libhw64/ide/isa.c ../libhw64/ide/piix.c ../libhw64/ide/ahci.c \
../libhw64/ide/ich.c ../libhw64/lsi53c895a.c ../libhw64/dma-helpers.c \
../libhw64/sysbus.c ../libhw64/isa-bus.c ../libhw64/qdev-addr.c \
../libhw64/vga-pci.c ../libhw64/vga-isa.c ../libhw64/vmware_vga.c \
../libhw64/vmmouse.c ../libhw64/sb16.c ../libhw64/es1370.c ../libhw64/ac97.c \
../libhw64/intel-hda.c ../libhw64/hda-audio.c ../trace.c vga.c mc146818rtc.c \
i8259.c pc.c cirrus_vga.c sga.c apic.c ioapic.c piix_pci.c vmport.c \
device-hotplug.c pci-hotplug.c smbios.c wdt_ib700.c debugcon.c multiboot.c \
pc_piix.c \
-Wl,-z,execheap,--verbose \
-O2 -g  \
-Wl,--warn-common \
-shared -g \
-o qemu \
-Wl,"-z execheap \
-Bstatic \
-L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib \
-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
-l:libSDL.a -l:libstdc++.a \
-L/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0 \
-l:libgcc.a \
crt0.o \
-Bdynamic \
-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
-ldl -lc -lm -lz \
"


if [ -e qemu ]; then
arm-unknown-linux-gnueabi-strip qemu
cp qemu /home/nathan/android/SDLapp/libs/armeabi/libnativeapp.so
fi


#CFLAGS="$CFLAGS -mandroid --no-exceptions -nostdlib \
#-I/home/nathan/android/SDL-1.2.14/include \
#-I/home/nathan/android/gcc4x/arm-linux-elf/include/SDL \
#-I/home/nathan/android/gcc4x/arm-linux-elf/include" \
#LDFLAGS="-z execheap \
#-Bstatic \
#-L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib \
#-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
#-l:libSDL.a -l:libstdc++.a \
#-L/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0 \
#-l:libgcc.a \
#crt0.o \
#-Bdynamic \
#-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
#-ldl -lc -lm -lz \
#" \

#-Bdynamic -lstdc++ -l:liblog.so -l:libdl.so -lm -lz -l:libSDL.so -l:libc.so.6 
#-l:libm.so.6
#-lstdc++ -l:liblog.so -l:libdl.so -lm -lz -l:libSDL.so \
#-l:libc.so.6 -l:libm.so.6 -l:libgcc_s.so.1 \
# -lutil -lglib-2.0 -lrt 
#-lz,-l:libc.so.6,-l:libm.so.6,-l:libgcc_s.a,\
#,--no-copy-dt-needed-entries,\
#-l:libc.so.6,-l:libm.so.6,-l:libgcc_s.so.1,-lstdc++,-lc,-l:liblog.so,-l:libdl.so,\
#-lm,-lz,-lutil,-lglib-2.0,-lrt,\
#-lSDL,-pthread,-lpthread
#-L/usr/arm-unknown-linux-gnueabi/lib,-lutil,-ldl,-lrt,-lz,-lm,-lSDL,-pthread,-lpthread \
#-L/home/nathan/android/gcc4x/arm-linux-elf/lib,\
#-L/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3 \
#-L/usr/lib/gcc/arm-unknown-linux-gnueabi/4.5.3 \
#-L/usr/arm-unknown-linux-gnueabi/lib \
#-L/home/nathan/android/SDL-1.2.14/build \
#-l:libc.so.6 -l:libm.so.6 -l:libdl.so.2 -l:libdl.so -lcutils 
#-l:libstdc++.so -l:libc.so \
#-l:ld-linux.so.3 -l:liblog.so -lutil -l:libgcc_s.so.1 
#
#-L/usr/arm-unknown-linux-gnueabi/lib \


#PATH=$toolpath:$PATH \
#LDFLAGS="$LDFLAGS \
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib/crtbegin_dynamic.o \
#-L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib -lc -nostdlib " \
#CFLAGS="$CFLAGS -nostdlib" \
#make

