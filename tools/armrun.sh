qemu-system-aarch64 -cpu cortex-a72      \
            -M virt                      \
            -m 512m                      \
            -bios meta/firmware/OVMF.fd  \
            -monitor stdio               \
            -device ramfb                \
            -device qemu-xhci            \
            -device usb-kbd              \
            -d int                       \
            Ignis.iso
