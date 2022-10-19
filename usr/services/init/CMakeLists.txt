file(GLOB_RECURSE CFILES "src/*.c")

set(CMAKE_C_FLAGS_INIT " ")
set (CMAKE_C_FLAGS "-fno-stack-protector \
-fno-pic -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
-Werror=int-conversion -Wno-builtin-declaration-mismatch -no-pie \
-Werror=incompatible-pointer-types -Werror=int-to-pointer-cast \
-nostdlib -nostdinc")


add_executable(init_service ${CFILES})
# target_link_libraries(init_service ${PROJECT_SOURCE_DIR}/libc/libalec.a)
set_target_properties(init_service PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/meta/initrd/initd.sys")
set_target_properties(init_service PROPERTIES LINK_FLAGS "-nostdlib -nostdinc -zmax-page-size=0x1000 -T${PROJECT_SOURCE_DIR}/usr/link.ld")
