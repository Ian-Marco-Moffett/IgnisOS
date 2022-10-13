# IgnisOS

## Building for x86
Install the following:

- cmake
- xorriso
- qemu
- gcc
- nasm


Now do ``bash build.sh``


## Building for aarch64
Install the same stuff needed for an x86 build but add:
- aarch-elf-gcc
- aarch-elf-as
<br>

Now edit build.sh and change the AARCH64 variable on the top
from ``false`` to ``true``, save and exit and do bash build.sh
