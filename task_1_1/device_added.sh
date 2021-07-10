#!/bin/bash
mkdir /tmp_mnt
mount /dev/sdb1 /tmp_mnt
cp var/log/boot.log /tmp_mnt
cp var/log/dmesg /tmp_mnt
umount /dev/sdb1
rm -rf /tmp_mnt
