failure() {
  echo -e "\n\nFIX YOUR ERRORS DUDE!\n"
  cd ../
  rm -rf bfiles
  exit
}

mkinitrd() {
  mv meta/initrd/ ./
  tar -cvf meta/internals/initrd.sys initrd/
  mv initrd/ meta/
}

mkdir -p meta/internals/
mkdir -p meta/initrd/
mkdir -p bfiles/
cd bfiles/; cmake ../ $@; make || failure
cd ../
rm -rf bfiles
git clone https://github.com/limine-bootloader/limine.git --branch=v3.0-branch-binary --depth=1
make -C limine
rm -rf iso_root
mkdir -p iso_root
mkdir -p iso_root/Ignis
cp meta/limine.cfg \
limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
cp kernel/kernel.sys meta/internals/* iso_root/Ignis/
xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o Ignis.iso
limine/limine-deploy Ignis.iso
rm -rf iso_root
rm -rf limine
