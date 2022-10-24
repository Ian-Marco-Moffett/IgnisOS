  .text
  .global _start
  .type   _start, @function
_start:

  call    main

  jmp .
  .size   _start, .-_start
