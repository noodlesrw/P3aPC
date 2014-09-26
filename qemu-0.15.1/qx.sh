#!/bin/bash

../qemu-0.15.1/i386-softmmu/qemu-i386-sdl -L ../a-qemu/sdcard/SDL/rom.2011 -m 768 -net nic,vlan=0 -net user,vlan=0 -localtime -soundhw sb16 -k en-us -hda ../virtualPCs/hdd0002.vdi -cdrom ../isos/ReactOS-BootCD.iso -boot d
