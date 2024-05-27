#ifndef __CPUID_C__
#define __CPUID_C__

#include "cpuid.h"

uint64_t rdtsc() {
    /*
     * El valor obtenido mediante la instrucción rdtsc puede variar debido a varias razones, incluyendo la tecnología de Turbo Boost de 
     * Intel, que aumenta dinámicamente la frecuencia del procesador por encima de su frecuencia base.
     * El valor de rdtsc está relacionado con la frecuencia base de la CPU y no necesariamente con la frecuencia instantánea actual, que puede estar 
     * afectada por Turbo Boost. Para obtener una medición precisa de la frecuencia actual del procesador, puedes usar una combinación de la 
     * instrucción rdtsc y las funciones del sistema operativo que proporcionan la frecuencia actual de la CPU, o hacer uso de las instrucciones 
     * CPUID para obtener información sobre el Turbo Boost.
     * Apartir del Pentium Pro se empezo a realizar el "out-of-order execution", lo que quiere decir que "rdtsc" puede ejecutarse ante de lo previsto
     * haciendo que la marca de tiempo ya no sea confiable.
     * Para procesadores Pentium M (familia [06H], modelos [09H, 0DH]); para procesadores Pentium 4, procesadores Intel Xeon (familia [0FH], modelos 
     *      [00H, 01H o 02H]); y para procesadores de la familia P6: el contador de marca de tiempo aumenta con cada ciclo de reloj interno del procesador. 
     *      El ciclo de reloj interno del procesador está determinado por la relación actual entre el reloj del núcleo y el reloj del bus. Las transiciones 
     *      de la tecnología Intel SpeedStep también pueden afectar el reloj del procesador.
     * Para procesadores Pentium 4, procesadores Intel Xeon (familia [0FH], modelos [03H y superiores]); para procesadores Intel Core Solo 
     *      e Intel Core Duo (familia [06H], modelo [0EH]); para el procesador Intel Xeon serie 5100 y los procesadores Intel Core 2 Duo 
     *      (familia [06H], modelo [0FH]); para procesadores Intel Core 2 e Intel Xeon (familia [06H], display_model [17H]); 
     *      para procesadores Intel Atom (familia [06H], display_model [1CH]): el contador de marca de tiempo aumenta a un ritmo constante. 
     *      Esa velocidad puede establecerse mediante la relación máxima entre el reloj del núcleo y el reloj del bus del procesador o puede 
     *      establecerse según la frecuencia máxima resuelta a la que se inicia el procesador. La frecuencia máxima resuelta puede diferir de 
     *      la frecuencia máxima calificada del procesador.
     * 
     * La configuración específica del procesador determina el comportamiento. El comportamiento constante del TSC garantiza que la duración de 
     * cada tic del reloj sea uniforme y permite utilizar el TSC como temporizador de pared incluso si el núcleo del procesador cambia de frecuencia. 
     * Este es el comportamiento arquitectónico de todos los procesadores Intel posteriores.
     * Los procesadores AMD hasta el núcleo K8 siempre incrementaban el contador de marca de tiempo en cada ciclo de reloj.
     * Por lo tanto, las funciones de administración de energía pudieron cambiar la cantidad de incrementos por segundo y los valores podrían 
     * desincronizarse entre diferentes núcleos o procesadores en el mismo sistema. Para Windows, AMD proporciona una utilidad[7] para sincronizar 
     * periódicamente los contadores en CPU de múltiples núcleos. Desde la familia 10h (Barcelona/Phenom), los chips AMD cuentan con un TSC constante, 
     * que puede ser controlado por la velocidad HyperTransport o por el estado P más alto. Un bit CPUID (Fn8000_0007:EDX_8) anuncia esto; Las CPU 
     * Intel también informan su TSC invariante en ese bit.
     */
    unsigned int lo, hi;
    __asm__ volatile (
        "rdtsc"
        : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

// Función para esperar un tiempo (no precisa)
void wait(int seconds) {
    for (int i = 0; i < seconds * 100000000; i++) {
        __asm__ volatile ("nop");
    }
}


/*
 *
 *  Los valores devuelto por get_flags pueden no ser completos por no estar en modo kernel
 * 
 */
#ifdef __x86_64__
uint64_t get_flags() {
    uint64_t flags;

    __asm__ volatile (
        "pushfq\n\t"         // Guarda el valor actual de RFLAGS en la pila
        "popq %0\n\t"        // Recupera el valor de RFLAGS en la variable 'flags'
        : "=rm" (flags)      // Usamos "rm" en lugar de "r" para permitir que flags se almacene en memoria
        :
        : "cc"
    );

    return flags;
}
#else
uint32_t get_flags() {
    uint32_t flags;

    __asm__ volatile (
        "pushfl\n\t"         // Guarda el valor actual de EFLAGS en la pila
        "popl %0\n\t"        // Recupera el valor de EFLAGS en la variable 'flags'
        : "=rm" (flags)      // Usamos "rm" en lugar de "r" para permitir que flags se almacene en memoria
        :
        : "cc"
    );

    return flags;
}
#endif


int is_cpuid_supported() {
    /*
     *
     * Determinar si se puede usar CPUID via flag ID de EFLAG/RFLAGS, puede requerir prvilegios
     * a nivel kernel, para poder averiguar en ese caso si se admite la instruccion, podemos modificar
     * el bit 21(ID), en caso de que se ocasione una excepcion, la CPU no admite esta instruccion,
     * de caso contrario, la CPU admite la instruccion CPUID, pero la flag ID no sera visible desde
     * el modo usuario. 
     * 
     */
#ifdef __x86_64__
    uint64_t flags1, flags2;

    __asm__ volatile (
        "pushfq\n\t"             // Guarda el valor actual de RFLAGS en la pila
        "popq %0\n\t"            // Recupera el valor de RFLAGS en la variable 'rflags1'
        "movq %0, %1\n\t"        // Copia rflags1 en rflags2
        "orq $0x200000, %0\n\t" // Alterna el bit 21 de rflags1
        "pushq %0\n\t"           // Guarda el valor modificado en la pila
        "popfq\n\t"              // Restaura el valor modificado en RFLAGS
        "pushfq\n\t"             // Guarda el valor actual de RFLAGS en la pila
        "popq %1\n\t"            // Recupera el valor actual en rflags2
        : "=r" (flags1), "=r" (flags2) // Cambio de "=r" (flags1), "=r" (flags2) a "=r" (flags1), "=rm" (flags2)
        :
        : "cc"
    );
#else
    uint32_t flags1, flags2;

    __asm__ volatile (
        "pushfl\n\t"             // Guarda el valor actual de EFLAGS en la pila
        "popl %0\n\t"            // Recupera el valor de EFLAGS en la variable 'eflags1'
        "movl %0, %1\n\t"        // Copia eflags1 en eflags2
        "orl $0x200000, %0\n\t" // Alterna el bit 21 de eflags1
        "pushl %0\n\t"           // Guarda el valor modificado en la pila
        "popfl\n\t"              // Restaura el valor modificado en EFLAGS
        "pushfl\n\t"             // Guarda el valor actual de EFLAGS en la pila
        "popl %1\n\t"            // Recupera el valor actual en eflags2
        : "=r" (flags1), "=r" (flags2) // Cambio de "=r" (flags1), "=r" (flags2) a "=r" (flags1), "=rm" (flags2)
        :
        : "cc"
    );
#endif
    return (flags1 >> 21); 
}



static inline uint8_t get_flags_bit(uint8_t bit) {
    return (get_flags() >> bit) & 1; 
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

void call_cpuid(uint32_t eax_in, uint32_t ecx_in, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    __asm__ volatile (
        "cpuid"
        : "=a" (*a), "=b" (*b), "=c" (*c), "=d" (*d)
        : "a" (eax_in), "c" (ecx_in)
    );
}

static inline uint8_t Get_Processor_Family_ID(Processor_Info_and_Feature_Bits *MyProcessor_Info_and_Feature_Bits){
    return MyProcessor_Info_and_Feature_Bits->Model + (MyProcessor_Info_and_Feature_Bits->Extended_Model_ID << 4);
}
static inline uint16_t Get_CPUID_signature(Processor_Info_and_Feature_Bits *MyProcessor_Info_and_Feature_Bits){
    /* Ejemplo:
    CPUID signature:	671
    Family:	   0x06 (06h)
    Model:	   0x07 (07h)
    Stepping:  0x01 (01h)
    */
    return (MyProcessor_Info_and_Feature_Bits->Family_ID << 8) + 
           (MyProcessor_Info_and_Feature_Bits->Model     << 4) + 
           (MyProcessor_Info_and_Feature_Bits->Stepping_ID   );
}

void printInformation_Feature_Bits(uint32_t edx, uint32_t ecx) {
    cpuid_feat_ecx *MyCpuid_feat_ecx = (cpuid_feat_ecx*)&ecx;
    cpuid_feat_edx *MyCpuid_feat_edx = (cpuid_feat_edx*)&edx;

    printf(
        "uint32_t SSE3       :1 = 0x%x;\n"
        "uint32_t PCLMUL     :1 = 0x%x;\n"
        "uint32_t DTES64     :1 = 0x%x;\n"
        "uint32_t MONITOR    :1 = 0x%x;\n"
        "uint32_t DS_CPL     :1 = 0x%x;\n"
        "uint32_t VMX        :1 = 0x%x;\n"
        "uint32_t SMX        :1 = 0x%x;\n"
        "uint32_t EST        :1 = 0x%x;\n"
        "uint32_t TM2        :1 = 0x%x;\n"
        "uint32_t SSSE3      :1 = 0x%x;\n"
        "uint32_t CID        :1 = 0x%x;\n"
        "uint32_t SDBG       :1 = 0x%x;\n"
        "uint32_t FMA        :1 = 0x%x;\n"
        "uint32_t CX16       :1 = 0x%x;\n"
        "uint32_t XTPR       :1 = 0x%x;\n"
        "uint32_t PDCM       :1 = 0x%x;\n"
        "uint32_t RESERVADO  :1 = 0x%x;\n"
        "uint32_t PCID       :1 = 0x%x;\n"
        "uint32_t DCA        :1 = 0x%x;\n"
        "uint32_t SSE4_1     :1 = 0x%x;\n"
        "uint32_t SSE4_2     :1 = 0x%x;\n"
        "uint32_t X2APIC     :1 = 0x%x;\n"
        "uint32_t MOVBE      :1 = 0x%x;\n"
        "uint32_t POPCNT     :1 = 0x%x;\n"
        "uint32_t TSC        :1 = 0x%x;\n"
        "uint32_t AES        :1 = 0x%x;\n"
        "uint32_t XSAVE      :1 = 0x%x;\n"
        "uint32_t OSXSAVE    :1 = 0x%x;\n"
        "uint32_t AVX        :1 = 0x%x;\n"
        "uint32_t F16C       :1 = 0x%x;\n"
        "uint32_t RDRAND     :1 = 0x%x;\n"
        "uint32_t HYPERVISOR :1 = 0x%x;\n",
        MyCpuid_feat_ecx->SSE3,      MyCpuid_feat_ecx->PCLMUL, MyCpuid_feat_ecx->DTES64, MyCpuid_feat_ecx->MONITOR, 
        MyCpuid_feat_ecx->DS_CPL,    MyCpuid_feat_ecx->VMX,    MyCpuid_feat_ecx->SMX,    MyCpuid_feat_ecx->EST, 
        MyCpuid_feat_ecx->TM2,       MyCpuid_feat_ecx->SSSE3,  MyCpuid_feat_ecx->CID,    MyCpuid_feat_ecx->SDBG, 
        MyCpuid_feat_ecx->FMA,       MyCpuid_feat_ecx->CX16,   MyCpuid_feat_ecx->XTPR,   MyCpuid_feat_ecx->PDCM, 
        MyCpuid_feat_ecx->RESERVADO, MyCpuid_feat_ecx->PCID,   MyCpuid_feat_ecx->DCA,    MyCpuid_feat_ecx->SSE4_1, 
        MyCpuid_feat_ecx->SSE4_2,    MyCpuid_feat_ecx->X2APIC, MyCpuid_feat_ecx->MOVBE,  MyCpuid_feat_ecx->POPCNT, 
        MyCpuid_feat_ecx->TSC,       MyCpuid_feat_ecx->AES,    MyCpuid_feat_ecx->XSAVE,  MyCpuid_feat_ecx->OSXSAVE, 
        MyCpuid_feat_ecx->AVX,       MyCpuid_feat_ecx->F16C,   MyCpuid_feat_ecx->RDRAND, MyCpuid_feat_ecx->HYPERVISOR
    );

    printf(
        "uint32_t FPU        :1 = 0x%x;\n"
        "uint32_t VME        :1 = 0x%x;\n"
        "uint32_t DE         :1 = 0x%x;\n"
        "uint32_t PSE        :1 = 0x%x;\n"
        "uint32_t TSC        :1 = 0x%x;\n"
        "uint32_t MSR        :1 = 0x%x;\n"
        "uint32_t PAE        :1 = 0x%x;\n"
        "uint32_t MCE        :1 = 0x%x;\n"
        "uint32_t CX8        :1 = 0x%x;\n"
        "uint32_t APIC       :1 = 0x%x;\n"
        "uint32_t RESERVADO  :1 = 0x%x;\n"
        "uint32_t SEP        :1 = 0x%x;\n"
        "uint32_t MTRR       :1 = 0x%x;\n"
        "uint32_t PGE        :1 = 0x%x;\n"
        "uint32_t MCA        :1 = 0x%x;\n"
        "uint32_t CMOV       :1 = 0x%x;\n"
        "uint32_t PAT        :1 = 0x%x;\n"
        "uint32_t PSE36      :1 = 0x%x;\n"
        "uint32_t PSN        :1 = 0x%x;\n"
        "uint32_t CLFLUSH    :1 = 0x%x;\n"
        "uint32_t NX         :1 = 0x%x;\n"
        "uint32_t DS         :1 = 0x%x;\n"
        "uint32_t ACPI       :1 = 0x%x;\n"
        "uint32_t MMX        :1 = 0x%x;\n"
        "uint32_t FXSR       :1 = 0x%x;\n"
        "uint32_t SSE        :1 = 0x%x;\n"
        "uint32_t SSE2       :1 = 0x%x;\n"
        "uint32_t SS         :1 = 0x%x;\n"
        "uint32_t HTT        :1 = 0x%x;\n"
        "uint32_t TM         :1 = 0x%x;\n"
        "uint32_t IA64       :1 = 0x%x;\n"
        "uint32_t PBE        :1 = 0x%x;\n",        
        MyCpuid_feat_edx->FPU,  MyCpuid_feat_edx->VME,        MyCpuid_feat_edx->DE, 
        MyCpuid_feat_edx->PSE,  MyCpuid_feat_edx->TSC,        MyCpuid_feat_edx->MSR, 
        MyCpuid_feat_edx->PAE,  MyCpuid_feat_edx->MCE,        MyCpuid_feat_edx->CX8, 
        MyCpuid_feat_edx->APIC, MyCpuid_feat_edx->RESERVADO , MyCpuid_feat_edx->SEP, 
        MyCpuid_feat_edx->MTRR, MyCpuid_feat_edx->PGE,        MyCpuid_feat_edx->MCA, 
        MyCpuid_feat_edx->CMOV, MyCpuid_feat_edx->PAT,        MyCpuid_feat_edx->PSE36, 
        MyCpuid_feat_edx->PSN,  MyCpuid_feat_edx->CLFLUSH,    MyCpuid_feat_edx->NX, 
        MyCpuid_feat_edx->DS,   MyCpuid_feat_edx->ACPI,       MyCpuid_feat_edx->MMX, 
        MyCpuid_feat_edx->FXSR, MyCpuid_feat_edx->SSE,        MyCpuid_feat_edx->SSE2, 
        MyCpuid_feat_edx->SS,   MyCpuid_feat_edx->HTT,        MyCpuid_feat_edx->TM, 
        MyCpuid_feat_edx->IA64, MyCpuid_feat_edx->PBE          
    );

}

void printAdditional_Information_Feature_Bits(uint32_t ebx) {
    Additional_Information_Feature_Bits *MyAdditional_Information_Feature_Bits = (Additional_Information_Feature_Bits*)&ebx;
    printf(
        "Brand_Index         :8  = 0x%02x;\n"  
        "CLFLUSH             :8  = 0x%02x;\n"  
        "Max_ID_addressable  :8  = 0x%02x;\n"  
        "Local_APIC_ID       :8  = 0x%02x;\n",    
        MyAdditional_Information_Feature_Bits->Brand_Index,
        MyAdditional_Information_Feature_Bits->CLFLUSH,
        MyAdditional_Information_Feature_Bits->Max_ID_addressable,
        MyAdditional_Information_Feature_Bits->Local_APIC_ID
    );
}

void printProcessor_Info_and_Feature_Bits(uint32_t eax) {
    Processor_Info_and_Feature_Bits *MyProcessor_Info_and_Feature_Bits = (Processor_Info_and_Feature_Bits*)&eax;
    printf("EAX                :32 = 0b"); 
    printBits(sizeof(eax), &eax);
    // 0000  0000_0000 1011 00 00 0110 0111 0001
    printf(
        "Reserved1           :4  = 0x%02x;\n"  // 31 - 28
        "Extended_Family_ID  :8  = 0x%02x;\n"  // 27 - 20
        "Extended_Model_ID   :4  = 0x%02x;\n"  // 19 - 16
        "Reserved2           :2  = 0x%02x;\n"  // 15 - 14
        "Processor_Type      :2  = 0x%02x;\n"  // 13 - 12
        "Family_ID           :4  = 0x%02x;\n"  // 11 - 8
        "Model               :4  = 0x%02x;\n"  // 7  - 4
        "Stepping_ID         :4  = 0x%02x;\n", // 3  - 0
        MyProcessor_Info_and_Feature_Bits->Reserved1,
        MyProcessor_Info_and_Feature_Bits->Extended_Family_ID,
        MyProcessor_Info_and_Feature_Bits->Extended_Model_ID,
        MyProcessor_Info_and_Feature_Bits->Reserved2,
        MyProcessor_Info_and_Feature_Bits->Processor_Type,
        MyProcessor_Info_and_Feature_Bits->Family_ID,
        MyProcessor_Info_and_Feature_Bits->Model,
        MyProcessor_Info_and_Feature_Bits->Stepping_ID
    );
    printf(
        "All_Model              = 0x%02x;\n"
        "CPUID signature        = 0x%04x;\n", 
        Get_Processor_Family_ID(MyProcessor_Info_and_Feature_Bits),
        Get_CPUID_signature(MyProcessor_Info_and_Feature_Bits)
    );
}


int main() {


    size_t val = get_flags();
    printf("flags                 0x%x = ", val);
    printBits(sizeof(size_t), &val);
    printf("ID flag               0x%x\n", get_flags_bit(21));
    printf("is_cpuid_supported    0x%x\n", is_cpuid_supported());
    if((is_cpuid_supported() == 0)) {
        puts("instruccion CPUID no se soporta");
        return -1;
    } else puts("instruccion CPUID se soporta");
    if ((val & flags_mask_CPUID) == 0) { // si no se puede obetener la flag ID directamente, posiblemenete estemos en modo usuario
        puts("Usted esta en modo Usuario posiblemenete");
    }

    uint32_t eax, ebx, ecx, edx;
    int code = CPUID_GETVENDORSTRING;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %08x\n", eax);
    printf("EBX: %08x\n", ebx);
    printf("ECX: %08x\n", ecx);
    printf("EDX: %08x\n", edx);
    manufacturer_ID MyManufacturer_ID = {
        .EBX = ebx, .ECX = ecx,
        .EDX = edx, .final_string = '\0'
    };
    printf("Valor maximo de entrada EAX para CPUID: %08x\n", eax);
    printf("manufacturer_ID: %s\n", (char*)(&MyManufacturer_ID));

    code = CPUID_GETFEATURES;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printProcessor_Info_and_Feature_Bits(eax);
    printInformation_Feature_Bits(edx, ecx);
    printAdditional_Information_Feature_Bits(ebx);

    code = CPUID_GETTLB;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_GETSERIAL;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_GETSERIAL;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_INTEL;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY2;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_MONITOR_MWAIT_FEATURES;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_THERMAL_AND_POWER_MANAGEMENT;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_Extended_Features;
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(CPUID_Extended_Features, CPUID_Extended_Features_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(CPUID_Extended_Features, CPUID_Extended_Features_page_2, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 3\n", code);
    call_cpuid(CPUID_Extended_Features, CPUID_Extended_Features_page_3, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_XSAVE_FEATURES_AND_STATE_COMPONENTS;
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(code, CPUID_XSAVE_features_and_state_components_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(code, CPUID_XSAVE_features_and_state_components_page_2, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 3\n", code);
    call_cpuid(code, CPUID_XSAVE_features_and_state_components_page_3, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_SGX_CAPATIBLES;
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(code, CPUID_SGX_capabilities_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(code, CPUID_SGX_capabilities_page_2, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 3\n", code);
    call_cpuid(code, CPUID_SGX_capabilities_page_3, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_PROCESSOR_TRACE;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, CPUID_Processor_Trace_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_TSC_AND_CORE_CRYSTAL_FREQUENCY_INFORMATION;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %08x (denominator TSC/core crystal clock = %u)\n", eax, eax);
    printf("EBX: %08x (enumerator TSC/core crystal clock = %u)\n", ebx, ebx);
    printf("ECX: %08x (core crystal clock = %u Hz)\n", ecx, ecx);
    unsigned long long int TSCFreq = ecx * (ebx / eax);
    printf("TSCFreq = %x (TSCFreq = %u Hz = %f GHz)\n", TSCFreq, TSCFreq, (float)((float)TSCFreq/1e9));
    printf("EDX: %08x\n", edx);

    code = CPUID_PROCESSOR_AND_BUS_SPECIFICATION_FREQUENCIES_INFORMATION;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %08x (Base Frequency = %u MHz)\n", eax, eax & 0xffff);
    printf("EBX: %08x (Max Frequency = %u MHz)\n",  ebx, ebx & 0xffff);
    printf("ECX: %08x (Bus (Reference) Frequency = %u MHz)\n", ecx, ecx & 0xffff);
    printf("EDX: %08x\n", edx);

    /*uint64_t start_tsc, end_tsc;

    // Medir TSC al inicio
    start_tsc = rdtsc();

    // Esperar aproximadamente 1 segundo
    usleep(1000000); // Esperar 1 segundo (aproximadamente)

    // Medir TSC al final
    end_tsc = rdtsc();

    // Calcular la diferencia en TSC
    uint64_t tsc_diff = end_tsc - start_tsc;

    // Calcular la frecuencia en GHz
    double cpu_freq_ghz = (double)tsc_diff / 1e9; // 1 segundo = 1e9 ciclos

    printf("CPU Frequency: %.2f GHz\n", cpu_freq_ghz);*/

    return 0;
}
#endif