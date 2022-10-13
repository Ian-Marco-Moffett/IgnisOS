project(AlevoneOS C ASM)
enable_language(ASM)
cmake_minimum_required(VERSION 3.0)

include_directories("kernel/include/")


file(GLOB_RECURSE ASMFILES RELATIVE ${CMAKE_SOURCE_DIR} "kernel/src/*.asm")
file(GLOB_RECURSE ASMFILES_AARCH RELATIVE ${CMAKE_SOURCE_DIR} "kernel/src/*.aarch.s")
file(GLOB_RECURSE CFILES RELATIVE ${CMAKE_SOURCE_DIR} "kernel/src/*.c")
set_property(SOURCE ${ASMFILES} APPEND PROPERTY COMPILE_OPTIONS "-felf64")

if (NOT AARCH64)
  message("COMPILG FOR X86_64")
  set (CMAKE_C_FLAGS "-fexceptions -std=gnu11 -ffreestanding -fno-stack-protector \
  -fno-pic -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
  -Werror=int-conversion \
  -Werror=incompatible-pointer-types -Werror=int-to-pointer-cast \
  -mabi=sysv -mno-80387 -mno-mmx \
  -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -MMD")
  add_executable(kernel ${CFILES} ${ASMFILES})
  set_target_properties(kernel PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/kernel/kernel.sys")
  set_target_properties(kernel PROPERTIES LINK_FLAGS "-nostdlib -zmax-page-size=0x1000 -static -T${PROJECT_SOURCE_DIR}/kernel/linker.ld")
ENDIF(NOT AARCH64)

if (AARCH64)
  set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
  set(CMAKE_C_COMPILER "aarch64-elf-gcc")
  set(CMAKE_C_FLAGS "-fexceptions -std=gnu11 -ffreestanding -fno-stack-protector \
  -fno-pic -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
  -Werror=int-conversion \
  -Werror=incompatible-pointer-types -Werror=int-to-pointer-cast -fno-lto")
  message("COMPILING FOR AARCH64")
  add_executable(kernel ${CFILES} ${ASMFILES_AARCH})
  set_target_properties(kernel PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/kernel/kernel.sys")
  set_target_properties(kernel PROPERTIES LINK_FLAGS "-nostdlib -zmax-page-size=0x1000 -static -T${PROJECT_SOURCE_DIR}/kernel/linker-arm.ld")
ENDIF(AARCH64)
