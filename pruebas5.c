#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "cpuid.h"
#include "colors.h"

#define change_null_space(val) ((val >= 32) && (val <= 126))  ? val : ' '

typedef struct cpu_model_string {
    uint32_t eax_1;
    uint32_t ebx_1;
    uint32_t ecx_1;
    uint32_t edx_1;

    uint32_t eax_2;
    uint32_t ebx_2;
    uint32_t ecx_2;
    uint32_t edx_2;

    uint32_t eax_3;
    uint8_t final_string; // \0
} cpu_model_string;

void print_uint32_string(uint32_t reg) {
    char string_reg_data[5] = {
        *((uint8_t*)(&reg)),
        *((uint8_t*)(&reg)+1),
        *((uint8_t*)(&reg)+2),
        *((uint8_t*)(&reg)+3),
        0x0
    };

    putchar(change_null_space(*(string_reg_data+0)));   
    putchar(change_null_space(*(string_reg_data+1)));
    putchar(change_null_space(*(string_reg_data+2)));
    putchar(change_null_space(*(string_reg_data+3)));
}

void PrintLastError(DWORD errorCode) {
    LPSTR messageBuffer = NULL;
    
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    if (size) {
        // Eliminar el salto de línea al final del mensaje si existe
        if (size > 2 && messageBuffer[size - 2] == '\r' && messageBuffer[size - 1] == '\n') {
            messageBuffer[size - 2] = '\0';
        }
        
        printf("Error (code %lu): %s\n", errorCode, messageBuffer);
        LocalFree(messageBuffer);
    } else {
        printf("Error (code %lu): Unable to retrieve error message\n", errorCode);
    }
}

/*
 * "Processors" indica el número total de procesadores lógicos en ese nivel.
 * "Shift" se usa para extraer información del x2APIC ID. "Shift 07" es el valor de desplazamiento para 
 *      obtener un ID único en el siguiente nivel (probablemente el nivel de paquete).
 * Significado de los niveles:
 * - Nivel 0 (Type 1): Nivel de hilo (SMT) Simultaneous Multi-Threading
 * - Nivel 1 (Type 2): Nivel de núcleo
 * - Nivel 2 (Type 3): Nivel de paquete/socket (si existe, en caso de que exista 
 *          varias CPU's, el nivel 3 indicaria que procesador tiene este paquete y nucleo)
 * 
 *       Nivel 2 (Type 3), Processors X, Shift Y, x2APIC ID Z
 *          La presencia de un Type 3 indicaría que el sistema tiene múltiples paquetes (sockets).
 *          El número de procesadores (X) en este nivel representaría el total de núcleos lógicos en todos los paquetes combinados.
 * 
 *       Caso hipotetico:
 *          Level 0: Type 1, Processors  2, Shift 1, x2APIC ID 0
 *          Level 1: Type 2, Processors  8, Shift 3, x2APIC ID 0
 *          Level 2: Type 3, Processors 16, Shift 4, x2APIC ID 0
 * 
 *          2 hilos por núcleo          (Level 0)
 *          4 núcleos por paquete       (Level 1 muestra 8 procesadores,  que es 2 * 4)
 *          2 paquetes/sockets en total (Level 2 muestra 16 procesadores, que es 8 * 2)
 * 
 * Hilos por núcleo = Processors del Nivel 0
 * Núcleos por paquete = Processors del Nivel 1 / Processors del Nivel 0
 * Número de paquetes = Processors del Nivel 2 (si existe) / Processors del Nivel 1
 */
typedef struct {
    int level;
    int type;
    int processors;
    int shift;
    int x2apic_id;
} TopologyLevel;

#define MAX_CORES 24
#define MAX_LEVELS 3

typedef struct {
    int level;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} TopologyInfo;

typedef struct {
    uint8_t core_id;
    uint8_t local_apic_id;
    TopologyInfo levels[MAX_LEVELS];
} CoreTopology;

CoreTopology core_topologies[MAX_CORES] = {0};

uint8_t Locals_APICs[24] = {0};
uint8_t Core_actual = 0;

void analyze_topology2(uint8_t core_id) {
    uint32_t eax, ebx, ecx, edx;
    int level_count = 0;

    for (int level = 0; level < MAX_LEVELS; level++) {
        call_cpuid(0x0B, level, &eax, &ebx, &ecx, &edx);
        if (ebx == 0) break;

        core_topologies[core_id].levels[level_count].level = level;
        core_topologies[core_id].levels[level_count].eax = eax;
        core_topologies[core_id].levels[level_count].ebx = ebx;
        core_topologies[core_id].levels[level_count].ecx = ecx;
        core_topologies[core_id].levels[level_count].edx = edx;

        level_count++;
    }

    core_topologies[core_id].core_id = core_id;
    core_topologies[core_id].local_apic_id = Locals_APICs[core_id];
}

void print_topology_summary() {
    printf("\nTopology Summary:\n");
    for (int i = 0; i < MAX_CORES; i++) {
        if (core_topologies[i].core_id == 0 && i != 0) break;

        printf("Core %d (Local APIC ID: %d):\n", core_topologies[i].core_id, core_topologies[i].local_apic_id);
        for (int j = 0; j < MAX_LEVELS; j++) {
            TopologyInfo* info = &core_topologies[i].levels[j];
            if (info->level == 0 && j != 0) break;

            int type = (info->ecx >> 8) & 0xFF;
            int processors = info->ebx & 0xFFFF;
            int shift = info->eax & 0x1F;
            int x2apic_id = info->edx;

            printf("  Nivel %02d: Tipo %02d, HilosPorNucleo(CoresLogicos) %02d, Shift %02d, x2APIC ID %02d\n",
                   info->level, type, processors, shift, x2apic_id);
        }
        printf("\n");
    }
}


void analyze_topology() {
    uint32_t eax, ebx, ecx, edx;
    TopologyLevel levels[3] = {0};
    int level_count = 0;

    for (int level = 0; level < 3; level++) {
        call_cpuid(0x0B, level, &eax, &ebx, &ecx, &edx);
        if (ebx == 0) break;

        levels[level_count].level = level;
        levels[level_count].type = (ecx >> 8) & 0xFF;
        levels[level_count].processors = ebx & 0xFFFF;
        levels[level_count].shift = eax & 0x1F;
        levels[level_count].x2apic_id = edx;

        printf("Nivel %d: Tipo %d, HilosPorNucleo(CoresLogicos) %d, Shift %d, x2APIC ID %d\n",
               level, levels[level_count].type, levels[level_count].processors,
               levels[level_count].shift, levels[level_count].x2apic_id);

        level_count++;
    }

    if (level_count >= 2) {
        int threads_per_core = levels[0].processors;
        int cores_per_package = levels[1].processors / levels[0].processors;
        int num_packages = (level_count == 3) ? levels[2].processors / levels[1].processors : 1;

        printf("Topology: %d Package(s), %d Core(s) per Package, %d Thread(s) per Core\n",
               num_packages, cores_per_package, threads_per_core);
        printf("Total Logical Processors: %d\n", num_packages * cores_per_package * threads_per_core);
    } else {
        printf("Unable to determine complete topology\n");
    }
}

int main() {
    // Obtén el handle del proceso actual
    HANDLE hProcess = GetCurrentProcess();

    uint32_t eax = 0, ebx = eax, ecx = eax, edx = eax;

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

        uint32_t max_cpuid = 0, max_cpuid_extended = 0;

        int code = CPUID_GETVENDORSTRING;
        printf("Call Cpuid With code: 0x%x \n", code);
        call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
        printf("EAX: %08x ", eax);
        printf("EBX: %08x\n", ebx);
        printf("ECX: %08x ", ecx);
        printf("EDX: %08x\n", edx);
        manufacturer_ID MyManufacturer_ID = {
            .EBX = ebx, .ECX = ecx,
            .EDX = edx, .final_string = '\0'
        };
        printf("Valor maximo de entrada EAX para CPUID: %08x\n", eax);
        printf("manufacturer_ID: %s\n", (char*)(&MyManufacturer_ID));
        max_cpuid = eax;

    // Define la máscara de afinidad para el segundo núcleo (núcleo 1)
    // 2 en binario es 10, lo que representa el segundo núcleo
    for (uint32_t processAffinityMask = 1; processAffinityMask < 0b1000000000000000000000000; processAffinityMask <<= 1, Core_actual++) {
        // Establece la afinidad del proceso
        if (SetProcessAffinityMask(hProcess, processAffinityMask))
        {
            printf_color("Core %hhu, Afinidad del proceso establecida correctamente. #{FG:lpurple}#{i32:%d}#{FG:reset}\n", Core_actual, processAffinityMask);
        }
        else
        {
            DWORD err = GetLastError();
            printf("Error al establecer la afinidad del proceso. Codigo de error: 0x%p, mascara: #{FG:lpurple}#{i32:%d}#{FG:reset}\n", err, processAffinityMask);
            PrintLastError(err);
            exit(-1);
        }

        printf("Call Cpuid With code: 0x%x \n", 0x1);
        call_cpuid(0x1, 0, &eax, &ebx, &ecx, &edx);
        printAdditional_Information_Feature_Bits(ebx);
        Additional_Information_Feature_Bits *MyAdditional_Information_Feature_Bits = (Additional_Information_Feature_Bits*)&ebx;
        Locals_APICs[Core_actual] = MyAdditional_Information_Feature_Bits->Local_APIC_ID;

        analyze_topology();
        analyze_topology2(Core_actual);

        uint32_t level_type, level_cores, level_threads;
        for (int level = 0; ; level++) {
            /*
             * Análisis de topología
             * El código itera a través de los niveles de topología incrementando un contador 'level'.
             * Para cada nivel, llama a CPUID con EAX=0x0B y ECX=level
             * Analiza los resultados en EBX, ECX y EDX para determinar:
             *      - Tipo de nivel (hilo, núcleo, paquete)
             *      - Número de procesadores lógicos en ese nivel
             *      - ID x2APIC del procesador lógico actual
             */
            call_cpuid(0x0B, level, &eax, &ebx, &ecx, &edx);

            if (ebx == 0) break; // Si EBX es 0, no hay más niveles

            printf("\n\nCall Cpuid With code: 0xb\n");
            printf("EAX = 0x%08x EBX = 0x%08x ECX = 0x%08x EDX = 0x%08x\n", eax, ebx, ecx, edx);

            level_type = (ecx >> 8) & 0xFF;
            level_cores = ebx & 0xFFFF;

            switch (level_type) {
                case 1:
                    printf("\tThread level: %d logical processors\n", level_cores);
                    break;
                case 2:
                    printf("\tCore level: %d logical processors\n", level_cores);
                    break;
                default:
                    printf("\tPackage level: %d logical processors\n", level_cores);
                    break;
            }
        }

    }

    for (uint8_t i = 0; i < 24; i++) {
        printf("%02hhu-", i, Locals_APICs[i]);
    }
    for (uint8_t i = 0; i < 24; i++) {
        printf("\nCore %02hhu, Local APIC %02hhu\n", i, Locals_APICs[i]);
    }
    print_topology_summary();
    return 0;
}