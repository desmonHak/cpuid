/*
 * EAX=4 y EAX=Bh: topología de caché y subprocesos/núcleos de Intel
 * Estas dos hojas se utilizan para la topología del procesador (subproceso, núcleo, paquete) 
 * y la enumeración de la jerarquía de caché en procesadores multinúcleo (y con hiperproceso) 
 * de Intel.[73] A partir de 2013, AMD no utiliza estas hojas, pero tiene formas alternativas 
 * de realizar la enumeración de núcleos.[74]
 * 
 * A diferencia de la mayoría de las otras hojas de CPUID, la hoja Bh devolverá diferentes 
 * valores en EDX según el procesador lógico que ejecute la instrucción CPUID; el valor 
 * devuelto en EDX es en realidad el identificador x2APIC del procesador lógico. Sin embargo, 
 * el espacio de identificadores x2APIC no se asigna continuamente a procesadores lógicos; puede 
 * haber lagunas en la asignación, lo que significa que algunos identificadores x2APIC 
 * intermedios no necesariamente corresponden a ningún procesador lógico. En los otros 
 * registros se proporciona información adicional para asignar los identificadores x2APIC a 
 * los núcleos. Aunque la hoja Bh tiene subhojas (seleccionadas por ECX como se describe 
 * más adelante), el valor devuelto en EDX solo se ve afectado por el procesador lógico en 
 * el que se ejecuta la instrucción, pero no por la subhoja.
 * 
 * La topología de procesadores expuesta por la hoja Bh es jerárquica, pero con la extraña 
 * salvedad de que el orden de los niveles (lógicos) en esta jerarquía no necesariamente 
 * corresponde al orden en la jerarquía física (SMT/núcleo/paquete). Sin embargo, cada nivel 
 * lógico se puede consultar como una subhoja ECX (de la hoja Bh) para su correspondencia 
 * con un "tipo de nivel", que puede ser SMT, núcleo o "inválido". El espacio de identificación 
 * de nivel comienza en 0 y es continuo, lo que significa que si una identificación de nivel es 
 * inválida, todas las identificaciones de nivel superior también lo serán. El tipo de nivel 
 * se devuelve en los bits 15:08 de ECX, mientras que la cantidad de procesadores lógicos en el 
 * nivel consultado se devuelve en EBX. Finalmente, la conexión entre estos niveles y los 
 * identificadores x2APIC se devuelve en EAX[4:0] como la cantidad de bits que se deben 
 * desplazar para obtener un identificador único en el siguiente nivel.
 * 
 * A modo de ejemplo, un procesador Westmere de doble núcleo capaz de realizar hiperprocesamiento 
 * (es decir, que tenga dos núcleos y cuatro subprocesos en total) podría tener identificadores 
 * x2APIC 0, 1, 4 y 5 para sus cuatro procesadores lógicos. La hoja Bh (=EAX), subhoja 0 (=ECX) 
 * de CPUID podría, por ejemplo, devolver 100h en ECX, lo que significa que el nivel 0 describe 
 * la capa SMT (hiperprocesamiento), y devolver 2 en EBX porque hay dos procesadores lógicos 
 * (unidades SMT) por núcleo físico. El valor devuelto en EAX para esta subhoja 0 debería ser 
 * 1 en este caso, porque desplazar los identificadores x2APIC antes mencionados a la derecha 
 * por un bit da un número de núcleo único (en el siguiente nivel de la jerarquía de 
 * identificadores de nivel) y borra el bit de identificador SMT dentro de cada núcleo. 
 * Una forma más simple de interpretar esta información es que el último bit (bit número 0) del 
 * identificador x2APIC identifica la unidad SMT/hyperthreading dentro de cada núcleo en 
 * nuestro ejemplo. Avanzar a la subhoja 1 (haciendo otra llamada a CPUID con EAX=Bh y ECX=1) 
 * podría, por ejemplo, devolver 201h en ECX, lo que significa que este es un nivel de tipo 
 * núcleo, y 4 en EBX porque hay 4 procesadores lógicos en el paquete; EAX devuelto podría 
 * ser cualquier valor mayor que 3, porque sucede que el bit número 2 se usa para identificar 
 * el núcleo en el identificador x2APIC. Tenga en cuenta que el bit número 1 del identificador 
 * x2APIC no se usa en este ejemplo. Sin embargo, el EAX devuelto en este nivel bien podría 
 * ser 4 (y resulta ser así en un Clarkdale Core i3 5x0) porque eso también da un identificador 
 * único en el nivel de paquete (=0 obviamente) al cambiar el identificador x2APIC por 4 bits. 
 * Finalmente, puede que te preguntes qué nos puede decir la hoja EAX=4 que no hayamos 
 * descubierto ya. En EAX[31:26] devuelve los bits de máscara APIC reservados para un paquete; 
 * eso sería 111b en nuestro ejemplo porque los bits 0 a 2 se utilizan para identificar 
 * procesadores lógicos dentro de este paquete, pero el bit 1 también está reservado 
 * aunque no se utiliza como parte del esquema de identificación de procesador lógico. 
 * En otras palabras, los identificadores APIC 0 a 7 están reservados para el paquete, 
 * aunque la mitad de estos valores no se asignan a un procesador lógico.
 * 
 * La jerarquía de caché del procesador se explora observando las subhojas de la 
 * hoja 4. Los identificadores APIC también se utilizan en esta jerarquía para transmitir 
 * información sobre cómo los diferentes niveles de caché son compartidos por las unidades 
 * SMT y los núcleos. Para continuar con nuestro ejemplo, la caché L2, que es compartida por 
 * las unidades SMT del mismo núcleo pero no entre núcleos físicos en Westmere, se indica 
 * mediante EAX[26:14] configurado en 1, mientras que la información de que la caché L3 es 
 * compartida por todo el paquete se indica configurando esos bits en (al menos) 111b. Los 
 * detalles de la caché, incluido el tipo de caché, el tamaño y la asociatividad, se comunican 
 * a través de los otros registros de la hoja 4.
 * 
 * Tenga en cuenta que las versiones anteriores de la nota de aplicación Intel 485 contienen 
 * información engañosa, en particular con respecto a la identificación y el recuento de 
 * núcleos en un procesador multinúcleo;[75] los errores por malinterpretar esta información 
 * se han incorporado incluso en el código de muestra de Microsoft para usar CPUID, incluso 
 * para la edición 2013 de Visual Studio,[76] y también en la página sandpile.org para CPUID,[77] 
 * pero el código de muestra de Intel para idLa topología de procesadores que identifica [73] 
 * tiene la interpretación correcta, y el Manual del desarrollador de software de Intel actual 
 * tiene un lenguaje más claro. El código de producción multiplataforma 
 * (de código abierto) [78] de Wildfire Games también implementa la interpretación 
 * correcta de la documentación de Intel.
 * 
 * En una presentación de Intel de 2010 se ofrecen ejemplos de detección de topología 
 * que involucran procesadores Intel más antiguos (anteriores a 2010) que carecen de x2APIC 
 * (por lo tanto, no implementan la hoja EAX=Bh).[79] Tenga en cuenta que el uso de ese método 
 * de detección más antiguo en procesadores Intel de 2010 y más nuevos puede sobreestimar 
 * la cantidad de núcleos y procesadores lógicos porque el método de detección anterior 
 * asume que no hay espacios en el espacio de identificación de APIC, y esta suposición es 
 * violada por algunos procesadores más nuevos (comenzando con la serie Core i3 5x0), pero 
 * estos procesadores más nuevos también vienen con un x2APIC, por lo que su topología se 
 * puede determinar correctamente utilizando el método de hoja EAX=Bh.
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <windows.h>
#include "cpuid.h"
#include "./colors-C-C-plus-plus/colors.h"

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

uint8_t Locals_APICs[24] = {0};
uint8_t Core_actual = 0;
int main() {
    // Obtén el handle del proceso actual
    HANDLE hProcess = GetCurrentProcess();

    uint32_t eax = 0, ebx = eax, ecx = eax, edx = eax;

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

        printf("Call Cpuid With code: 0x%x \n", 0x1);
        call_cpuid(0x1, 0, &eax, &ebx, &ecx, &edx);
        printAdditional_Information_Feature_Bits(ebx);
        Additional_Information_Feature_Bits *MyAdditional_Information_Feature_Bits = (Additional_Information_Feature_Bits*)&ebx;
        Locals_APICs[Core_actual] = MyAdditional_Information_Feature_Bits->Local_APIC_ID;


        uint32_t level_type, level_cores, level_threads;
        for (int level = 0; ; level++) {
        call_cpuid(0x0B, level, &eax, &ebx, &ecx, &edx);

        if (ebx == 0) break; // Si EBX es 0, no hay más niveles

        printf("Call Cpuid With code: 0xb\n");
        printf("EAX = 0x%08x EBX = 0x%08x ECX = 0x%08x EDX = 0x%08x\n", eax, ebx, ecx, edx);

        level_type = (ecx >> 8) & 0xFF;
        level_cores = ebx & 0xFFFF;

        switch (level_type) {
            case 1:
                printf("Thread level: %d logical processors\n", level_cores);
                break;
            case 2:
                printf("Core level: %d logical processors\n", level_cores);
                break;
            default:
                printf("Package level: %d logical processors\n", level_cores);
                break;
        }
    }


        cpu_model_string MyCpuString = {0};
        call_cpuid(0x80000002, 0, &MyCpuString.eax_1, &MyCpuString.ebx_1, &MyCpuString.ecx_1, &MyCpuString.edx_1);
        call_cpuid(0x80000003, 0, &MyCpuString.eax_2, &MyCpuString.ebx_2, &MyCpuString.ecx_2, &MyCpuString.edx_2);
        call_cpuid(0x80000004, 0, &MyCpuString.eax_3, &ebx, &ecx, &edx);
        printf("cpu_model_string: %s\n", (char*)(&MyCpuString));

        code = 0x80000000;
        printf("Call Cpuid With code: 0x%x\n", code);
        call_cpuid(code, 0, &eax, &ebx, &ecx, &edx);
        printf("Valor maximo de entrada en EAX para CPUID en valores extendidos: %08x\n", eax);
        max_cpuid_extended = eax; 
        
        for (uint32_t i = 0; i <= max_cpuid; i++) {
            eax = 0, ecx = eax, ebx = ecx, edx = ecx; 
            call_cpuid(i, 0, &eax, &ebx, &ecx, &edx);
            printf_color("CPUID #{FG:lpurple}0x%08x#{FG:white}: #{FG:green}%08x#{FG:white}-#{FG:green}%08x#{FG:white}-#{FG:green}%08x#{FG:white}-#{FG:green}%08x ", i, eax, ebx, ecx, edx);
            print_uint32_string(ebx); 
            putchar('-');
            print_uint32_string(edx); 
            putchar('-');
            print_uint32_string(ecx); 
            putchar('-');
            print_uint32_string(eax); 
            printf_color(
                "\nEAX = #{FG:lyellow}0x%08x#{FG:reset} EBX = #{FG:lyellow}0x%08x#{FG:reset} "
                "ECX = #{FG:lyellow}0x%08x#{FG:reset} EDX = #{FG:lyellow}0x%08x#{FG:reset}\n",
                eax, ebx, ecx, edx
            );
        }

        for (uint32_t i = 0x80000000; i <= max_cpuid_extended; i++) {
            eax = 0, ecx = eax, ebx = ecx, edx = ecx; 
            call_cpuid(i, 0, &eax, &ebx, &ecx, &edx);
            printf_color("CPUID #{FG:lpurple}0x%08x#{FG:white}: #{FG:green}%08x#{FG:white}-#{FG:green}%08x#{FG:white}-#{FG:green}%08x#{FG:white}-#{FG:green}%08x ", i, eax, ebx, ecx, edx);
            print_uint32_string(ebx); 
            putchar('-');
            print_uint32_string(edx); 
            putchar('-');
            print_uint32_string(ecx); 
            putchar('-');
            print_uint32_string(eax); 
            printf_color(
                "\nEAX = #{FG:lyellow}0x%08x#{FG:reset} EBX = #{FG:lyellow}0x%08x#{FG:reset} "
                "ECX = #{FG:lyellow}0x%08x#{FG:reset} EDX = #{FG:lyellow}0x%08x#{FG:reset}\n",
                eax, ebx, ecx, edx
            );
        }
        //getchar();
    }

    for (uint8_t i = 0; i < 24; i++) {
        printf("Core %02hhu, Local APIC %02hhu\n", i, Locals_APICs[i]);
    }
    return 0;
}