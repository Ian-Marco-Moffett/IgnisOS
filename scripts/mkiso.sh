#!/bin/sh
git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
 
# Build limine-deploy.
make -C limine
 
# Create a directory which will be our ISO root.
mkdir -p iso_root
mkdir -p iso_root/Ignis
 
# Copy the relevant files over.
cp kernel/kernel.sys meta/internals/* iso_root/Ignis/
cp -v meta/limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
 
# Create the bootable ISO.
xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-cd-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o Ignis.iso
 
# Install Limine stage 1 and 2 for legacy BIOS boot.
./limine/limine-deploy Ignis.iso
