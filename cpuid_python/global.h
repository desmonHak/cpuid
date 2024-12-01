#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdarg.h>
#include <stdio.h>

#ifdef _MSC_VER
// Alias para las funciones MinGW
int __mingw_vsscanf(const char* str, const char* format, va_list ap)
{
    return vsscanf(str, format, ap);
}

int __mingw_vfprintf(FILE* stream, const char* format, va_list ap)
{
    return vfprintf(stream, format, ap);
}

int __mingw_vsnprintf(char* buffer, size_t count, const char* format, va_list ap)
{
    return vsnprintf(buffer, count, format, ap);
}

int __mingw_vsprintf(char* buffer, const char* format, va_list ap)
{
    return vsprintf(buffer, format, ap);
}
// son necesarios si se compila para el VS
#endif

#define __COLORS_C__
#include "../colors-C-C-plus-plus/colors.h"

#ifdef DEBUG_ENABLE_CPUID
#define debug_print_cpuid(msg, ...) printf_color("#{FG:lgreen}[#{FG:cyan}%s#{FG:reset}:#{FG:red}%03d#{FG:green}]#{FG:reset} " msg , __FILE__, __LINE__, ##__VA_ARGS__)

#else
#define debug_print_cpuid(msg, ...)
#endif



#endif