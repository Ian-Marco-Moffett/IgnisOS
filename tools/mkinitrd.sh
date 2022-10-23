mv meta/initrd/ ./
cd initrd/
tar -cvf ../meta/internals/initrd.sys *
cd ../
mv initrd/ meta/
