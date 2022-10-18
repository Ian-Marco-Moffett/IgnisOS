mkdir -p var/
mkdir -p var/log/
qemu-system-x86_64 --enable-kvm -cpu host -M q35 -m 3G -drive file=Ignis.iso -boot d -monitor stdio -smp 4 -rtc base=localtime -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0 -serial file:var/log/ignis.log
# qemu-system-x86_64 -d int -M q35 -m 3G -drive file=Ignis.iso -boot d -monitor stdio -serial telnet:localhost:4321,server,nowait -smp 4 -rtc base=localtime -audiodev pa,id=audio0 -machine pcspk-audiodev=audio0
