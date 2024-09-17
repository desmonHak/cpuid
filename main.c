#include "cpuid.h"
#include <windows.h>


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
    printf("Call Cpuid With code: 0x%x Page 1\n", code);
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

    code = 0x10;
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(code, 1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 3\n", code);
    call_cpuid(code, 2, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 4\n", code);
    call_cpuid(code, 3, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 5\n", code);
    call_cpuid(code, 4, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 6\n", code);
    call_cpuid(code, 5, &eax, &ebx, &ecx, &edx);
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
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(code, CPUID_Processor_Trace_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(code, CPUID_Processor_Trace_page_2, &eax, &ebx, &ecx, &edx);
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
    printf("EDX: %08x\n", edx); // edx siempre es 0

    /*
     *
     * NOTAS:
     * Los datos se devuelven desde esta interfaz de acuerdo con la especificación del procesador y no
     * reflejan valores reales. El uso adecuado de estos datos incluye la visualización de información del procesador en forma similar
     * manera a la cadena de marca del procesador y para determinar el rango apropiado a usar al mostrar información del procesador, 
     * p. Gráficos de historial de frecuencia. La información devuelta no debe no se podrá utilizar para ningún otro propósito, ya que 
     * la información devuelta no se correlaciona con precisión con la información/contadores devueltos por otras interfaces del procesador.
     * Si bien un procesador puede admitir la hoja de información de frecuencia del procesador, los campos que devuelven una
     * El valor de cero no es compatible.
     *
     */
    code = CPUID_PROCESSOR_AND_BUS_SPECIFICATION_FREQUENCIES_INFORMATION;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %08x (Base Frequency = %u MHz)\n", eax, eax & 0xffff);
    printf("EBX: %08x (Max Frequency = %u MHz)\n",  ebx, ebx & 0xffff);
    printf("ECX: %08x (Bus (Reference) Frequency = %u MHz)\n", ecx, ecx & 0xffff);
    printf("EDX: %08x\n", edx);

    //  metodo no fiable de medir el clock de la cpu!
    uint64_t start_tsc, end_tsc;

    // Medir TSC al inicio
    start_tsc = rdtsc();

    // Esperar aproximadamente 1 segundo
    Sleep(1000); // Esperar 1 segundo (aproximadamente)

    // Medir TSC al final
    end_tsc = rdtsc();

    // Calcular la diferencia en TSC
    uint64_t tsc_diff = end_tsc - start_tsc;

    // Calcular la frecuencia en GHz
    double cpu_freq_ghz = (double)tsc_diff / 1e9; // 1 segundo = 1e9 ciclos

    printf("CPU Frequency: %.2f GHz\n", cpu_freq_ghz);
    printf("tick's de inicio: %llu\n", start_tsc);
    printf("tick's de final: %llu\n", end_tsc);
    printf("tick's de final - inicio: %llu\n", end_tsc - start_tsc);

    printf("tiempo que a pasado: %llu\n", (end_tsc - start_tsc) / TSCFreq);

    start_tsc = rdtsc();
    end_tsc = rdtsc();
    printf("tick's de inicio: %llu\n", start_tsc);
    printf("tick's de final: %llu\n", end_tsc);
    printf("tick's de final - inicio: %llu\n", end_tsc - start_tsc);

    start_tsc = rdtsc();
    end_tsc = rdtsc();
    printf("tick's de inicio: %llu\n", start_tsc);
    printf("tick's de final: %llu\n", end_tsc);
    printf("tick's de final - inicio: %llu\n", end_tsc - start_tsc);

    
    code = CPUID_SOC_VENDOR_ATTRIBUTE_ENUMRATION;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_INTEL_KEY_LOCKER_FEATURES;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_RESERVED_FOR_TDX_ENUMERATION;
    printf("Call Cpuid With code: 0x%x page 1\n", code);
    call_cpuid(code, CPUID_AVX10_Features_page_1, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("Call Cpuid With code: 0x%x page 2\n", code);
    call_cpuid(code, CPUID_AVX10_Features_page_2, &eax, &ebx, &ecx, &edx);
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);

    code = CPUID_RESERVED_FOR_HYPERVISOR_USE;
    printf("Call Cpuid With code: 0x%x\n", code);
    call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
    MyManufacturer_ID = (manufacturer_ID){
        .EBX = ebx, .ECX = edx,
        .EDX = ecx, .final_string = '\0'
    };
    printf("EAX: %x\n", eax);
    printf("EBX: %x\n", ebx);
    printf("ECX: %x\n", ecx);
    printf("EDX: %x\n", edx);
    printf("manufacturer_ID: %s\n", (char*)(&MyManufacturer_ID));

    return 0;
}