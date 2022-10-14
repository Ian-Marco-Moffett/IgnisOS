#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_

#include <lib/asm.h>


_isr void __vec0(void* stackframe);
_isr void __vec1(void* stackframe);
_isr void __vec3(void* stackframe);
_isr void __vec4(void* stackframe);
_isr void __vec5(void* stackframe);
_isr void __vec6(void* stackframe);
_isr void __vec8(void* stackframe);
_isr void __vec10(void* stackframe);
_isr void __vec11(void* stackframe);
_isr void __vec12(void* stackframe);
_isr void __vec13(void* stackframe);
_isr void __vec14(void* stackframe);
void init_exceptions(void);


#endif
