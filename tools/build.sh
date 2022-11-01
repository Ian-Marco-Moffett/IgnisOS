failure() {
  echo -e "\n\nFIX YOUR ERRORS DUDE!\n"
  cd ../
  rm -rf bfiles
  exit
}

firmware() {
  mkdir -p sbin/ovmf
	cd sbin/ovmf && curl -f -o OVMF-AA64.zip https://efi.akeo.ie/OVMF/OVMF-AA64.zip && unzip -f OVMF-AA64.zip
	rm sbin/ovmf/readme.txt &&	rm sbin/ovmf/OVMF-AA64.zip
  cd ../../
}


mkdir -p meta/internals/
mkdir -p meta/initrd/
mkdir -p bfiles/
bash tools/mklib.sh
export ASM="nasm -felf64"
cd bfiles/; cmake -DLIVE=1 ../ $@; make || failure
cd ../
bash tools/mkinitrd.sh

# Only build limine once
if [[ ! -d limine ]]
then
  git clone https://github.com/limine-bootloader/limine.git --branch=v4.0-binary --depth=1
  make -C limine
fi

if [[ ! -f meta/disk.img ]]
then
  echo "CREATING DISK IMAGE.."
  qemu-img create meta/disk.img 1M
fi


rm -rf iso_root
mkdir -p iso_root
mkdir -p iso_root/Ignis
cp etc/limine-live.cfg ./
mv limine-live.cfg limine.cfg
cp limine.cfg \
limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
cp sys/kernel-live.sys meta/internals/* iso_root/Ignis/
xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o IgnisInstaller.iso
limine/limine-deploy IgnisInstaller.iso


# Build actual system ISO.
cp etc/limine.cfg \
limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
cp sys/kernel.sys meta/internals/* iso_root/Ignis/
xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o meta/initrd/Ignis.iso
limine/limine-deploy meta/initrd/Ignis.iso
# rm -rf iso_root
