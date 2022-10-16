#ifndef STRING_H_
#define STRING_H_

#include <lib/types.h>

size_t kstrlen(const char* str);
bool kmemcmp(const char* str1, const char* str2, size_t n);
char* dec2str(int dec);
uint8_t* kstrncpy(uint8_t* dst, const uint8_t* src, const uint8_t len);
void kmemcpy(uint8_t* dst, const uint8_t* src, size_t len);
uint8_t* hex2str(uint64_t hex_num);
void kmemzero(void* ptr, size_t n);
void kmemset(void* ptr, uint64_t data, size_t n);
uint8_t kstrcmp(const char* str1, const char* str2);
uint8_t kstrncmp(const char* str1, const char* str2, size_t n);
uint64_t hex2int(char* hex, size_t len);
void utf8_strncpy(const wchar_t* src, wchar_t* dst, size_t n);
size_t utf8_strlen(const wchar_t* utf8_str);


#endif // STRING_H_
