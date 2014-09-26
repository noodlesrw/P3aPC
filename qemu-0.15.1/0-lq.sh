#!/bin/bash

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

arm-unknown-linux-gnueabi-gcc \
-D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
-Wstrict-prototypes -Wredundant-decls -Wall -Wundef -Wwrite-strings \
-Wmissing-prototypes -fno-strict-aliasing -I/home/nathan/android/SDL-1.2.14/include  \
-fstack-protector-all -Wendif-labels -Wmissing-include-dirs -Wempty-body \
-Wnested-externs -Wformat-security -Wformat-y2k -Winit-self -Wignored-qualifiers \
-Wold-style-declaration -Wold-style-definition -Wtype-limits \
-DHAS_AUDIO -DHAS_AUDIO_CHOICE   -DTARGET_PHYS_ADDR_BITS=64 \
-I../linux-headers -I.. -I/home/nathan/android/qemu-0.15.1/target-i386 -DNEED_CPU_H  \
-I/usr/arm-unknown-linux-gnueabi/usr/include/glib-2.0 \
-I/usr/arm-unknown-linux-gnueabi/usr/lib/glib-2.0/include \
arch_init.o cpus.o monitor.o machine.o gdbstub.o balloon.o virtio.o virtio-blk.o \
virtio-balloon.o virtio-net.o virtio-serial-bus.o vhost_net.o rwhandler.o \
kvm-stub.o xen-stub.o ../cutils.o ../cache-utils.o ../qemu-malloc.o \
../qemu-option.o ../module.o ../async.o ../nbd.o ../block.o ../aio.o ../aes.o \
../qemu-config.o ../qemu-progress.o ../qemu-sockets.o ../posix-aio-compat.o \
../block/raw.o ../block/cow.o ../block/qcow.o ../block/vdi.o ../block/vmdk.o \
../block/cloop.o ../block/dmg.o ../block/bochs.o ../block/vpc.o ../block/vvfat.o \
../block/qcow2.o ../block/qcow2-refcount.o ../block/qcow2-cluster.o \
../block/qcow2-snapshot.o ../block/qcow2-cache.o ../block/qed.o \
../block/qed-gencb.o ../block/qed-l2-cache.o ../block/qed-table.o \
../block/qed-cluster.o ../block/qed-check.o ../block/parallels.o ../block/nbd.o \
../block/blkdebug.o ../block/sheepdog.o ../block/blkverify.o ../block/raw-posix.o .\
./blockdev.o ../net.o ../net/queue.o ../net/checksum.o ../net/util.o \
../net/socket.o ../net/dump.o ../net/tap.o ../net/tap-linux.o ../qint.o \
../qstring.o ../qdict.o ../qlist.o ../qfloat.o ../qbool.o ../qjson.o \
../json-lexer.o ../json-streamer.o ../json-parser.o ../qerror.o ../error.o \
../readline.o ../console.o ../cursor.o ../qemu-error.o ../osdep.o \
../oslib-posix.o ../qemu-thread-posix.o ../os-posix.o ../tcg-runtime.o \
../host-utils.o ../irq.o ../ioport.o ../input.o ../i2c.o ../smbus.o \
../smbus_eeprom.o ../eeprom93xx.o ../scsi-disk.o ../cdrom.o \
../scsi-generic.o ../scsi-bus.o ../usb.o ../usb-hub.o ../usb-linux.o \
../usb-hid.o ../usb-msd.o ../usb-wacom.o ../usb-serial.o ../usb-net.o \
../usb-bus.o ../usb-desc.o ../bt.o ../bt-host.o ../bt-vhci.o ../bt-l2cap.o \
../bt-sdp.o ../bt-hci.o ../bt-hid.o ../usb-bt.o ../bt-hci-csr.o \
../buffered_file.o ../migration.o ../migration-tcp.o ../qemu-char.o \
../savevm.o ../msmouse.o ../ps2.o ../qdev.o ../qdev-properties.o \
../block-migration.o ../iohandler.o ../pflib.o ../bitmap.o ../bitops.o \
../migration-exec.o ../migration-unix.o ../migration-fd.o \
../audio/audio.o ../audio/noaudio.o ../audio/wavaudio.o ../audio/mixeng.o \
../audio/sdlaudio.o ../audio/wavcapture.o ../ui/keymaps.o \
../ui/sdl.o ../ui/sdl_zoom.o ../ui/x_keymap.o ../iov.o ../acl.o \
../compatfd.o ../notify.o ../event_notifier.o ../qemu-timer.o \
../qemu-timer-common.o ../libdis/i386-dis.o exec.o translate-all.o \
cpu-exec.o translate.o tcg/tcg.o fpu/softfloat.o op_helper.o helper.o \
cpuid.o disas.o ../libhw64/vl.o ../libhw64/loader.o ../libhw64/virtio-console.o \
../libhw64/virtio-pci.o ../libhw64/fw_cfg.o ../libhw64/pci.o \
../libhw64/pci_bridge.o ../libhw64/msix.o ../libhw64/msi.o ../libhw64/pci_host.o \
../libhw64/pcie_host.o ../libhw64/ioh3420.o ../libhw64/xio3130_upstream.o \
../libhw64/xio3130_downstream.o ../libhw64/watchdog.o ../libhw64/serial.o \
../libhw64/parallel.o ../libhw64/i8254.o ../libhw64/pcspk.o ../libhw64/pckbd.o \
../libhw64/usb-uhci.o ../libhw64/usb-ohci.o ../libhw64/usb-ehci.o ../libhw64/fdc.o \
../libhw64/acpi.o ../libhw64/acpi_piix4.o ../libhw64/pm_smbus.o ../libhw64/apm.o \
../libhw64/dma.o ../libhw64/hpet.o ../libhw64/applesmc.o ../libhw64/wdt_i6300esb.o \
../libhw64/pcie.o ../libhw64/pcie_aer.o ../libhw64/pcie_port.o ../libhw64/ne2000.o \
../libhw64/eepro100.o ../libhw64/pcnet-pci.o ../libhw64/pcnet.o ../libhw64/e1000.o \
../libhw64/rtl8139.o ../libhw64/ne2000-isa.o ../libhw64/ide/core.o \
../libhw64/ide/atapi.o ../libhw64/ide/qdev.o ../libhw64/ide/pci.o \
../libhw64/ide/isa.o ../libhw64/ide/piix.o ../libhw64/ide/ahci.o \
../libhw64/ide/ich.o ../libhw64/lsi53c895a.o ../libhw64/dma-helpers.o \
../libhw64/sysbus.o ../libhw64/isa-bus.o ../libhw64/qdev-addr.o \
../libhw64/vga-pci.o ../libhw64/vga-isa.o ../libhw64/vmware_vga.o \
../libhw64/vmmouse.o ../libhw64/sb16.o ../libhw64/es1370.o ../libhw64/ac97.o \
../libhw64/intel-hda.o ../libhw64/hda-audio.o ../trace.o vga.o mc146818rtc.o \
i8259.o pc.o cirrus_vga.o sga.o apic.o ioapic.o piix_pci.o vmport.o \
device-hotplug.o pci-hotplug.o smbios.o wdt_ib700.o debugcon.o multiboot.o \
pc_piix.o \
-Wl,-z,execheap,--verbose \
-O2 -g  \
-Wl,--warn-common \
-shared -g \
-o qemu \
-Wl,-Bstatic \
-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
-L/home/nathan/android/gcc4x/lib \
-L/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0 \
-lSDLmain -lSDL_mixer -lSDL -liberty -lm -lc -lthread_db \
-Wl,-Bdynamic \
-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
-L/home/nathan/android/gcc4x/lib \
-L/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0 \
-lm -lc -lz -llog 




if [ -e qemu ]; then
arm-unknown-linux-gnueabi-strip qemu
cp qemu /home/nathan/android/SDLapp/libs/armeabi/libnativeapp.so
fi

