project(AlevoneOS C ASM_NASM)
enable_language(ASM_NASM)
cmake_minimum_required(VERSION 3.0)

SET(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
include_directories("kernel/include/")


file(GLOB_RECURSE ASMFILES RELATIVE ${CMAKE_SOURCE_DIR} "kernel/src/*.asm")
file(GLOB_RECURSE CFILES RELATIVE ${CMAKE_SOURCE_DIR} "kernel/src/*.c")

set (CMAKE_C_FLAGS "-fexceptions -std=gnu11 -ffreestanding -fno-stack-protector \
-fno-pic -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
-Werror=int-conversion \
-Werror=incompatible-pointer-types -Werror=int-to-pointer-cast \
-mabi=sysv -mno-80387 -mno-mmx \
-mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -MMD")

if (VERBOSE)
  ADD_DEFINITIONS(-DNEUTRINO_VERBOSE=1)
ENDIF(VERBOSE)

add_executable(kernel ${ASMFILES} ${CFILES})
set_target_properties(kernel PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/kernel/kernel.sys")
set_target_properties(kernel PROPERTIES LINK_FLAGS "-nostdlib -zmax-page-size=0x1000 -static -T${PROJECT_SOURCE_DIR}/kernel/linker.ld")
