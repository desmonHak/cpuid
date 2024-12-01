#include <stdio.h>
#include <stdint.h>
#include "cpuid.h"
#include "./colors-C-C-plus-plus/colors.h"
/*
 * Obtener cuanta cache se comparte por core
 * 
 * 
 */

int main() {
    uint32_t eax, ebx, ecx, edx;
    int cacheLevel = 0; // 0 para L1, 1 para L2, 2 para L3, etc.
    
    for (; cacheLevel < 3; cacheLevel++){
        // Llamada a CPUID con EAX=4 y ECX=cacheLevel
        call_cpuid(4, cacheLevel, &eax, &ebx, &ecx, &edx);
        
        // Extraer los bits 25:14 de EAX
        uint32_t maxLogicalProcessors = (eax >> 14) & 0xFFF;
        
        // Sumar 1
        maxLogicalProcessors += 1;
        
        printf("Numero maximo de procesadores logicos que comparten la cache de nivel L%d: %u\n", 
            cacheLevel + 1, maxLogicalProcessors);
    }
    return 0;
}
