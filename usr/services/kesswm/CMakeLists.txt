file(GLOB_RECURSE CFILES "src/*.c")

set(CMAKE_C_FLAGS_INIT " ")
set (CMAKE_C_FLAGS "-fno-stack-protector \
-fno-pic -Werror=implicit -Werror=implicit-function-declaration -Werror=implicit-int \
-Werror=int-conversion -Wno-builtin-declaration-mismatch -no-pie \
-Werror=incompatible-pointer-types -Werror=int-to-pointer-cast -Wunused \
-nostdlib -nostdinc -I${PROJECT_SOURCE_DIR}/etc/headerconf/ -I${PROJECT_SOURCE_DIR}/usr/lib/libc/include/ -I${PROJECT_SOURCE_DIR}/usr/lib/libgfx/include/ -I${CMAKE_CURRENT_SOURCE_DIR}/include/")


add_executable(kesswm ${CFILES})
target_link_libraries(kesswm ${PROJECT_SOURCE_DIR}/usr/lib/libc/libignis.a ${PROJECT_SOURCE_DIR}/usr/lib/libgfx/libgfx.a)
set_target_properties(kesswm PROPERTIES OUTPUT_NAME "${PROJECT_SOURCE_DIR}/meta/initrd/kesswm.sys")
set_target_properties(kesswm PROPERTIES LINK_FLAGS "-nostdlib -nostdinc -zmax-page-size=0x1000 -T${PROJECT_SOURCE_DIR}/usr/link.ld")
