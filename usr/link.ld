ENTRY(main)

SECTIONS {
    . = 0x3000;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    .rodata : {
      *(.rodata)
    }

    /DISCARD/ : {
        *(.note .note.*)
    }
}
