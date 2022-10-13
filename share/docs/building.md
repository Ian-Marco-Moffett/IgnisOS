## Building for x86
Install the following:

- cmake
- xorriso
- qemu
- gcc
- nasm


Now do ``bash tools/build.sh``


## Building for aarch64 (WORK IN PROGRESS, USE X86 INSTEAD)
Install the same stuff needed for an x86 build but add:
- aarch-elf-gcc
- aarch-elf-as
<br>

Now edit tools/build.sh and change the AARCH64 variable on the top
from ``false`` to ``true``, save and exit and do ``bash tools/build.sh``
