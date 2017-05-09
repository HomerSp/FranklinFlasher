# FranklinFlasher
Flasher tool for flashing Franklin-based MiFis

Instructions for creating firmware.sfp:
sudo mksquashfs * ../../rootfs.squashfs -comp xz -noappend -b 262144
Update version.inf with new sha512sum
GZIP=-9 tar -cvzf firmware-test.sfp version.inf acpu bcpu rootfs.squashfs boot_order
