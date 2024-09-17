#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "../colors.h"

#ifdef DEBUG_ENABLE_CPUID
#define debug_print_cpuid(msg, ...) printf_color("#{FG:lgreen}[#{FG:cyan}%s#{FG:reset}:#{FG:red}%03d#{FG:green}]#{FG:reset} " msg , __FILE__, __LINE__, ##__VA_ARGS__)

#else
#define debug_print_cpuid(msg, ...)
#endif



#endif