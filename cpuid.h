#ifndef __CPUID_H__
#define __CPUID_H__

// https://www.cpu-world.com/cgi-bin/CPUID.pl?s=21
// https://en.wikipedia.org/wiki/CPUID
// Intel® Architecture Instruction Set Extensions and Future Features Programming Reference

#include <stdio.h>
#include <stdint.h>

/*
 *
 * https://github.com/jamesstringerparsec/Easy-GPU-PV/issues/226
 * ¿Hay alguna forma de modificar el bit del hipervisor para evitar esas detecciones de VM?
 * Esto es simple en VMware configurando "hypervisor.cpuid.v0 = "FALSE"" en el archivo vmx.
 *
 */
typedef enum flags_mask { // https://en.wikipedia.org/wiki/FLAGS_register
    flags_mask_Carry_flag                = 0x00000001, // flag CF   (Status)   1bit  (bit 0)
    flags_mask_Reserved_EFLAGS           = 0x00000002, // siempre en 1 en EFLAGS    (bit 1)
    flags_mask_Parity_flag               = 0x00000004, // flag PF   (Status)   1bit  (bit 2)
    flags_mask_Reserved1                 = 0x00000008, // (reservada)                (bit 3) https://www.righto.com/2013/02/looking-at-silicon-to-understanding.html
    flags_mask_Auxiliary_Carry_flag      = 0x00000010, // Flag AF   (Status)   1bit  (bit 4)
    flags_mask_Reserved2                 = 0x00000020, // (reservada)                (bit 5) https://www.righto.com/2013/02/looking-at-silicon-to-understanding.html
    flags_mask_Zero_flag                 = 0x00000040, // Flag ZF   (Status)   1bit  (bit 6)
    flags_mask_Sign_flag                 = 0x00000080, // Flag DF   (Status)   1bit  (bit 7)
    flags_mask_Trap_flag                 = 0x00000100, // Flag TF   (Control)  1bit  (bit 8)
    flags_mask_Direction_flag            = 0x00000200, // Flag IF   (Control)  1bit  (bit 9)
    flags_mask_Overflow_flag             = 0x00000400, // Flag DF   (Control)  1bit  (bit 10)
    flags_mask_Interrupt_enable_flag     = 0x00000800, // Flag OF   (Status)   1bit  (bit 11)
    flags_mask_IO_privilege_level        = 0x00003000, // Flag IOPL (System)   2bits (bit 12-13)
    flags_mask_Nested_task_flag          = 0x00004000, // Flag NT   (System)   1bit  (bit 14)
    flags_mask_Mode_flag                 = 0x00008000, // Flag MD   (Control)  1bit  (bit 15)
    // flags_mask_Mode_flag: Indicador de modo (solo serie NEC V), reservado en todas las CPU Intel. Siempre 1 en 8086/186, 0 en 286 y posteriores
   
    // IA-32   
    flags_mask_Resume_flag               = 0x00010000, // Flag RF   (System)   1bit  (bit 16)
    flags_mask_Virtual_8086_mode         = 0x00020000, // Flag VM   (System)   1bit  (bit 17)
    flags_mask_Alignment_Check           = 0x00040000, // Flag AC   (System)   1bit  (bit 18)
    // Verificación de alineación (486+, ring 3), Comprobación de acceso SMAP (Broadwell+, ring 0-2)
    flags_mask_Virtual_interrupt_flag    = 0x00080000, // Flag VIF  (System)   1bit  (bit 19)
    flags_mask_Virtual_interrupt_pending = 0x00100000, // Flag VIP  (System)   1bit  (bit 20)
    flags_mask_CPUID                     = 0x00200000, // Flag ID   (System)   1bit  (bit 21) "Para CPUID"
    flags_mask_Reserved3                 = 0x3FC00000, // (reservada)          8bits (bit 22–29)    
    flags_mask_AES_key                   = 0x40000000, // (None)    (System)   1bit  (bit 30) "Bandera cargada de programación
    //  de claves AES (CPU con VIA PadLock únicamente)" -> https://en.wikipedia.org/wiki/VIA_PadLock
    flags_mask_Alternate_Instruction_Set = 0x80000000, // Flag AI   (System)   1bit  (bit 31) "Conjunto de instrucciones alternativo habilitado
    // (Solo procesadores VIA C5XL)[7] -> https://en.wikipedia.org/wiki/Alternate_Instruction_Set

} flags_mask;

typedef enum cpuid_requests {
    CPUID_GETVENDORSTRING                  = 0x00000000,
    CPUID_GETFEATURES,
    CPUID_GETTLB,
    CPUID_GETSERIAL,                          // informacion de serie, se retiro en la mayoria de CPU's
                                              //  por seguridad
    CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_INTEL, // Jerarquía y topología de caché (para intel)
    /*
     * Estas dos hojas se utilizan para proporcionar información sobre los niveles de 
     * jerarquía de caché disponibles para el núcleo del procesador en el que se ejecuta 
     * la instrucción CPUID. La hoja 4 se usa en procesadores Intel y la hoja 8000001Dh se usa en procesadores AMD; 
     */
    CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY1 = CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_INTEL, // para procesadores intel
    // es te valor es el mismo que CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_INTEL, pero para procesadores amd son distintos al ocupar 
    // la pagina CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_AMD

    CPUID_MONITOR_MWAIT_FEATURES                = 0x05,
    // Esto devuelve información de funciones relacionadas con las instrucciones MONITOR y MWAIT en los registros EAX, EBX, ECX y EDX.
    /* 
     * CPUID EAX=5: MONITOR/MWAIT feature information in EAX, EBX, EDX
     *  _________________________________________________________________________________________________________________________________________________
     * | Bit   |                 EAX                                   EBX                    |                      EDX	                    | Bit    |
     * |_______|______________________________________________________________________________|_________________________________________________|________|
     * | 3:0   |  Smallest monitor-line size in bytes. | Largest monitor-line size in bytes.  |  Number of C0[a] sub-states supported for MWAIT | 3:0    |
     * | 7:4   |                                       |                                      |  Number of C1    sub-states supported for MWAIT | 7:4    |
     * | 11:8  |                                       |                                      |  Number of C2    sub-states supported for MWAIT | 11:8   |
     * | 15:12 |                                       |                                      |  Number of C3    sub-states supported for MWAIT | 15:12  |
     * |_______|_______________________________________|______________________________________|_________________________________________________|________|
     * | 19:16 |             (reserved)                |             (reserved)               |  Number of C4 sub-states supported for MWAIT    |  19:16 |
     * | 23:20 |                                       |                                      |  Number of C5 sub-states supported for MWAIT    |  23:20 |
     * | 27:24 |                                       |                                      |  Number of C6 sub-states supported for MWAIT    |  27:24 |
     * | 31:28 |                                       |                                      |  Number of C7 sub-states supported for MWAIT    |  31:28 |
     * |_______|_______________________________________|______________________________________|_________________________________________________|________|
     *  __________________________________________________________________________________________________________________________________________________
     * | Bit  |                                                                      ECX                                                                  |
     * |      |___________________________________________________________________________________________________________________________________________|
     * |      |     Short	        |                                              Feature                                                                | 
     * |______|_____________________|_____________________________________________________________________________________________________________________|
     * | 0	  |     EMX	            |           Enumeration of MONITOR/MWAIT extensions in ECX and EDX supported                                          |                       
     * |______|_____________________|_____________________________________________________________________________________________________________________|
     * | 1	  |     IBE	            |           Supports treating interrupts as break-events for MWAIT even when interrupts are disabled                  |                                               
     * |______|_____________________|_____________________________________________________________________________________________________________________|
     * | 2	  |                                                                  (reserved)                                                               |  
     * |______|___________________________________________________________________________________________________________________________________________|
     * | 3	  | Monitorless_­MWAIT   |       Allow MWAIT to be used for power management without setting up memory monitoring with MONITOR[80]             |                                                     
     * |______|_____________________|_____________________________________________________________________________________________________________________|
     * | 31:4 |	                                                                 (reserved)                                                               |  
     * |______|___________________________________________________________________________________________________________________________________________|
     *              
     */
    CPUID_THERMAL_AND_POWER_MANAGEMENT          = 0x6,

    CPUID_Extended_Features, // se espera recibir un valor en ECX que sea un valor del enum CPUID_Extended_Features_page

    CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY2 = 0x0b,
    /*
     * A diferencia de la mayoría de las otras hojas de CPUID, la hoja Bh devolverá diferentes valores en EDX dependiendo del 
     * procesador lógico que ejecute la instrucción CPUID; el valor devuelto en EDX es en realidad la identificación x2APIC del 
     * procesador lógico. Sin embargo, el espacio de identificación x2APIC no se asigna continuamente a procesadores lógicos; Puede haber 
     * lagunas en el mapeo, lo que significa que algunos identificadores x2APIC intermedios no necesariamente corresponden a ningún procesador 
     * lógico. En los otros registros se proporciona información adicional para asignar los identificadores de x2APIC a los núcleos. Aunque la 
     * hoja Bh tiene subhojas (seleccionadas por ECX como se describe más adelante), el valor devuelto en EDX solo se ve afectado por el procesador 
     * lógico en el que se ejecuta la instrucción, pero no por la subrehoja.
     * La topología del procesador expuesta por la hoja Bh es jerárquica, pero con la extraña advertencia de que el orden de los niveles (lógicos) 
     * en esta jerarquía no corresponde necesariamente al orden en la jerarquía física (SMT/núcleo/paquete) . Sin embargo, cada nivel lógico se 
     * puede consultar como una subhoja ECX (de la hoja Bh) para determinar su correspondencia con un "tipo de nivel", que puede ser SMT, 
     * núcleo o "no válido". El espacio de identificación de nivel comienza en 0 y es continuo, lo que significa que si una identificación 
     * de nivel no es válida, todas las identificaciones de niveles superiores también lo serán. El tipo de nivel se devuelve en los bits 
     * 15:08 de ECX, mientras que el número de procesadores lógicos en el nivel consultado se devuelve en EBX. Finalmente, la conexión entre 
     * estos niveles y los identificadores x2APIC se devuelve en EAX[4:0] como el número de bits que el identificador x2APIC debe desplazarse 
     * para obtener un identificador único en el siguiente nivel.
     * Como ejemplo, un procesador Westmere de doble núcleo capaz de hyperthreading (por lo tanto, con dos núcleos y cuatro subprocesos en total) 
     * podría tener los identificadores x2APIC 0, 1, 4 y 5 para sus cuatro procesadores lógicos. Hoja Bh (=EAX), subhoja 0 (=ECX) de CPUID podría, 
     * por ejemplo, devolver 100h en ECX, lo que significa que el nivel 0 describe la capa SMT (hyperthreading) y devolver 2 en EBX porque hay dos 
     * procesadores lógicos (unidades SMT) por núcleo físico. El valor devuelto en EAX para esta subhoja 0 debe ser 1 en este caso, porque desplazar 
     * los identificadores x2APIC antes mencionados hacia la derecha un bit proporciona un número central único (en el siguiente nivel de la jerarquía 
     * de identificadores de nivel) y borra el identificador de SMT. poco dentro de cada núcleo. Una forma más sencilla de interpretar esta información es 
     * que el último bit (número de bit 0) de la identificación x2APIC identifica la unidad SMT/hyperthreading dentro de cada núcleo en nuestro ejemplo. 
     * Avanzar a la subleaf 1 (haciendo otra llamada a CPUID con EAX=Bh y ECX=1) podría, por ejemplo, devolver 201h en ECX, lo que significa que este es un 
     * nivel de tipo de núcleo, y 4 en EBX porque hay 4 procesadores lógicos en el paquete; EAX devuelto podría tener cualquier valor mayor que 3, porque 
     * sucede que el bit número 2 se usa para identificar el núcleo en la identificación x2APIC. Tenga en cuenta que el bit número 1 de la identificación 
     * x2APIC no se utiliza en este ejemplo. Sin embargo, EAX devuelto en este nivel bien podría ser 4 (y sucede que es así en un Clarkdale Core i3 5x0) 
     * porque eso también proporciona una identificación única en el nivel del paquete (=0 obviamente) al cambiar la identificación de x2APIC en 4 bits. 
     * Finalmente, quizás se pregunte qué nos puede decir la hoja EAX=4 que no hayamos descubierto ya. En EAX[31:26] devuelve los bits de máscara APIC 
     * reservados para un paquete; eso sería 111b en nuestro ejemplo porque los bits 0 a 2 se usan para identificar procesadores lógicos dentro de este 
     * paquete, pero el bit 1 también está reservado aunque no se usa como parte del esquema de identificación del procesador lógico. En otras palabras, 
     * los ID de APIC del 0 al 7 están reservados para el paquete, aunque la mitad de estos valores no se asignan a un procesador lógico.
     * 
     * La jerarquía de caché del procesador se explora observando las subhojas de la hoja 4. Los identificadores de APIC también se utilizan en esta jerarquía 
     * para transmitir información sobre cómo las unidades y núcleos SMT comparten los diferentes niveles de caché. Para continuar con nuestro ejemplo, la 
     * caché L2, que es compartida por unidades SMT del mismo núcleo pero no entre núcleos físicos en Westmere, se indica estableciendo EAX[26:14] en 1, 
     * mientras que la información de que la caché L3 se comparte por todo el paquete se indica estableciendo esos bits en (al menos) 111b. Los detalles de 
     * la caché, incluido el tipo, el tamaño y la asociatividad de la caché, se comunican a través de los otros registros en la hoja 4.
     * 
     * Tenga en cuenta que las versiones anteriores de la nota de aplicación Intel 485 contienen información engañosa, particularmente con respecto a la 
     * identificación y el recuento de núcleos en un procesador multinúcleo; [75] incluso se han incorporado errores por malinterpretar esta información 
     * en el código de muestra de Microsoft para usar CPUID , incluso para la edición 2013 de Visual Studio,[76] y también en la página sandpile.org para 
     * CPUID,[77] pero el ejemplo de código de Intel para identificar la topología del procesador[73] tiene la interpretación correcta, y el actual Intel 
     * Software Developer's El manual tiene un lenguaje más claro. El código de producción multiplataforma (de código abierto)[78] de Wildfire Games también 
     * implementa la interpretación correcta de la documentación de Intel.
     * 
     * En una presentación de Intel de 2010 se dan ejemplos de detección de topología que involucran procesadores Intel más antiguos (anteriores a 2010) que 
     * carecen de x2APIC (por lo tanto, no implementan la hoja EAX=Bh). Tenga en cuenta que el uso de ese método de detección anterior en procesadores Intel 
     * 2010 y más nuevos puede sobreestimar la cantidad de núcleos y procesadores lógicos porque el método de detección anterior supone que no hay espacios 
     * en el espacio de identificación APIC, y algunos procesadores más nuevos violan esta suposición (comenzando con la serie Core i3 5x0), pero estos 
     * procesadores más nuevos también vienen con un x2APIC, por lo que su topología se puede determinar correctamente utilizando el método de hoja EAX=Bh.
     */

    CPUID_XSAVE_FEATURES_AND_STATE_COMPONENTS         = 0xD, // se espera recibir en ECX un valor enum CPUID_XSAVE_features_and_state_components_page
    /*
     *
     * Esta hoja se utiliza para enumerar características y componentes de estado de XSAVE.
     * La extensión del conjunto de instrucciones XSAVE está diseñada para guardar/restaurar el estado extendido de la CPU (normalmente con el fin de cambiar de 
     * contexto) de una manera que se puede ampliar para cubrir nuevas extensiones del conjunto de instrucciones sin que el código de cambio de contexto del 
     * sistema operativo necesite comprender los detalles del nuevas extensiones. Esto se hace definiendo una serie de componentes de estado, cada uno con 
     * un tamaño y desplazamiento dentro de un área de guardado determinada, y cada uno correspondiente a un subconjunto del estado necesario para una extensión 
     * de CPU u otra. La hoja EAX=0Dh CPUID se utiliza para proporcionar información sobre qué componentes de estado admite la CPU y cuáles son sus 
     * tamaños/compensaciones, de modo que el sistema operativo pueda reservar la cantidad adecuada de espacio y configurar los bits de habilitación asociados.
     * 
     * Los componentes de estado se pueden subdividir en dos grupos: estado de usuario (elementos de estado que son visibles para la aplicación, por ejemplo, 
     * registros vectoriales AVX-512) y estado de supervisor (elementos de estado que afectan a la aplicación pero que no son directamente del usuario). 
     * visible, por ejemplo, configuración de interrupción en modo de usuario). Los elementos de estado de usuario se habilitan configurando sus bits 
     * asociados en el registro de control XCR0, mientras que los elementos de estado de supervisor se habilitan configurando sus bits asociados en el 
     * MSR IA32_XSS (0DA0h); los elementos de estado indicados se convierten en los componentes de estado que se puede guardar y restaurar con la familia 
     * de instrucciones XSAVE/XRSTOR.
     * 
     * El mecanismo XSAVE puede manejar hasta 63 componentes de estado de esta manera. Los componentes de estado 0 y 1 (x87 y SSE, respectivamente) tienen 
     * compensaciones y tamaños fijos; para los componentes de estado 2 a 62, sus tamaños, compensaciones y algunos indicadores adicionales se pueden consultar 
     * ejecutando CPUID con EAX=0Dh y ECX configurado en el índice del componente estatal. Esto devolverá los siguientes elementos en EAX, EBX y ECX (con EDX reservado):
     * 
     */
    CPUID_SGX_CAPATIBLES                        = 0x12, // se espera recibir en ECX un valor enum CPUID_SGX_capabilities_page
    /*
     * Esta hoja proporciona información sobre las capacidades admitidas de la función Intel Software Guard Extensions (SGX). La hoja proporciona múltiples subhojas, 
     *  seleccionadas con ECX.
     * 
     * La subhoja 0 proporciona información sobre las funciones de hoja SGX admitidas en EAX y los tamaños máximos de enclave SGX admitidos en EDX; ECX está reservado. 
     * EBX proporciona un mapa de bits de bits que se pueden configurar en el campo MISCSELECT en SECS (Estructura de control de enclave SGX). Este campo se utiliza 
     * para controlar la información escrita en la región MISC de SSA (Área de estado de guardado SGX) cuando un AEX (SGX Se produce una salida asincrónica del enclave).
     * 
     */

    CPUID_PROCESSOR_TRACE                       = 0x14, // se espera recibir en ECX un valor enum CPUID_Processor_Trace_page
    /*
     *
     * Esta subhoja proporciona información sobre las funciones de Intel Processor Trace (también conocido como seguimiento de instrucciones en tiempo real).
     * 
     * El valor devuelto en EAX es el índice de la subhoja más alta admitida para CPUID con EAX=14h. EBX y ECX proporcionan indicadores de funciones, EDX está reservado.
     * 
     */

    CPUID_TSC_AND_CORE_CRYSTAL_FREQUENCY_INFORMATION = 0X15,
    /*
     *
     * CPUID EAX=15h: información de frecuencia de TSC y Core Crystal EAX EBX ECX Relación entre la frecuencia del TSC y la frecuencia del reloj del cristal central, 
     * denominador Relación entre la frecuencia del TSC y la frecuencia del reloj del cristal central, numerador[a] Frecuencia del reloj del cristal central, en unidades 
     * de Hz[a]
     * CPUID EAX=15h: TSC and Core Crystal frequency information
     *   __________________________________________________________________________________________________________________________________
     *  |                   EAX	                |                    EBX                 |                   ECX                           |                         
     *  |_______________________________________|________________________________________|_________________________________________________|
     *  |Ratio of TSC frequency to Core Crystal | Ratio of TSC frequency to Core Crystal | Core Crystal Clock frequency, in units of Hz[a] |
     *  |Clock frequency, denominator	        |   Clock frequency, numerator[a]        |                                                 |    
     *  |_______________________________________|________________________________________|_________________________________________________|        
     * 
     */

    CPUID_PROCESSOR_AND_BUS_SPECIFICATION_FREQUENCIES_INFORMATION = 0X16,
    /*
     *
     * CPUID EAX=16h: Frecuencias de especificación de procesador y bus[b]
     * Bits EAX EBX ECX
     * 15:0 Frecuencia base del procesador (en MHz)[a] Frecuencia máxima del procesador (en MHz)[a] Frecuencia de bus/referencia (en MHz)[a] 15:0
     * 31:16 (reservado) (reservado) (reservado) 31:16
     *   Campo no enumerado si es cero.
     *   Los valores de frecuencia informados por la hoja 16h son las frecuencias de especificación del procesador; son constantes para el procesador dado y no 
     * reflejan necesariamente la velocidad real del reloj de la CPU en el momento en que se llama a CPUID.
     * Si los valores devueltos en EBX y ECX de la hoja 15h son distintos de cero, entonces la frecuencia del TSC (Contador de marca de tiempo) en Hz viene dada por 
     * TSCFreq = ECX*(EBX/EAX).
     *   __________________________________________________________________________________________________________________________________________
     *  | Bits  |                   EAX	                |                    EBX                 |                   ECX                           |                         
     *  |_______|_______________________________________|________________________________________|_________________________________________________|
     *  | 15:0  |   Processor Base Frequency (in MHz)   |  Processor Maximum Frequency (in MHz)  |         Bus/Reference frequency (in MHz)        | 
     *  |_______|_______________________________________|________________________________________|_________________________________________________| 
     *  | 31:16 |             (reserved)                |                 (reserved)             |                     (reserved)                  | 
     *  |_______|_______________________________________|________________________________________|_________________________________________________| 
     * 
     */

    CPUID_SOC_VENDOR_ATTRIBUTE_ENUMRATION       = 0x17,
    /*
     *
     * Esta hoja está presente en sistemas donde un núcleo IP de CPU x86 está implementado en un SoC (sistema en chip) de otro proveedor; mientras que 
     * las otras hojas de CPUID brindan información sobre el núcleo de CPU x86, esta hoja proporciona información sobre el SoC. Esta hoja toma un índice 
     * de subhoja en ECX.
     * 
     */

    CPUID_INTEL_KEY_LOCKER_FEATURES             = 0x19,
    // Esta hoja proporciona información sobre las funciones de Intel Key Locker en EAX, EBX y ECX. EDX está reservado.

    CPUID_RESERVED_FOR_TDX_ENUMERATION          = 0x21,
    /*
     *
     * Cuando Intel TDX (Trust Domain Extensions) está activo, el módulo TDX interceptará los intentos de ejecutar la 
     * instrucción CPUID por parte de un invitado TD (Trust Domain). Este módulo, cuando se invoca CPUID con EAX=21h y ECX=0 (hoja 21h, subhoja 0), 
     * devolverá el índice de la subhoja más alta admitida para la hoja 21h en EAX y una cadena de identificación del proveedor del módulo TDX como 
     * Cadena ASCII de 12 bytes en EBX,EDX,ECX (en ese orden). La implementación del propio módulo de Intel devuelve la cadena de identificación del 
     * proveedor "IntelTDX" (con cuatro espacios finales)[102] - para este módulo, la información de características adicionales no está disponible 
     * a través de CPUID y debe obtenerse a través de la instrucción TDCALL específica de TDX.
     * 
     * Esta hoja está reservada en hardware y (en procesadores cuya hoja básica más alta sea 21 h o superior) 
     * devolverá 0 en EAX/EBX/ECX/EDX cuando se ejecute directamente en la CPU.
     * 
     */

    CPUID_AVX10_FEATURES                        = 0x24, // se espera recibir en ECX un valor enum CPUID_AVX10_Features_page
    // Esto devuelve una subhoja máxima admitida en EAX y información de características AVX10 en EBX.[94] (ECX y EDX están reservados).

    // Para hipervisores y maquinas virtuales -> EAX=40000000h-4FFFFFFFh: Reserved for Hypervisor use:
    /*
     *
     * Cuando la instrucción CPUID se ejecuta bajo la virtualización Intel VT-x o AMD-v, será interceptada por el hipervisor, lo que le permitirá devolver 
     * indicadores de características CPUID que difieren de los del hardware subyacente. Las hojas de CPUID 40000000h a 4FFFFFFFh no están implementadas 
     * en el hardware y están reservadas para que las utilicen los hipervisores para proporcionar identificación específica del hipervisor e información 
     * de características a través de este mecanismo de interceptación.
     * 
     * Para la hoja 40000000h, se espera que el hipervisor devuelva el índice de la hoja CPUID del hipervisor admitida más alta en EAX y una cadena de 
     * ID de hipervisor de 12 caracteres en EBX,ECX,EDX (en ese orden). Para la hoja 40000001h, el hipervisor puede devolver una firma de identificación 
     * de interfaz en EAX, p. Los hipervisores que deseen anunciar que son compatibles con Hyper-V pueden devolver 0x31237648—“Hv#1” en EAX.[103][104] 
     * Los formatos de las hojas 40000001h y hasta la hoja más alta admitida son específicos del hipervisor. Los hipervisores que implementan estas hojas 
     * normalmente también configurarán el bit 31 de ECX para la hoja 1 de CPUID para indicar su presencia.
     * 
     * Los hipervisores que exponen más de una interfaz de hipervisor pueden proporcionar conjuntos adicionales de hojas de CPUID para las interfaces 
     * adicionales, con un espaciado de hojas de 100 h por interfaz. Por ejemplo, cuando QEMU está configurado para proporcionar interfaces Hyper-V y 
     * KVM, proporcionará información de Hyper-V a partir de la hoja CPUID 40000000h e información de KVM a partir de la hoja 40000100h.[105][106]
     * 
     */
    CPUID_RESERVED_FOR_HYPERVISOR_USE           = 0x40000000,

    // CPUID extendidos, comprobar con CPUID eax = 0 el valor max de entrada admitido. Si el valor retornado en EAX no es superior, no se puede realizar estos
    // llamados.
    CPUID_INTELEXTENDED                         = 0x80000000,
    CPUID_INTELFEATURES,
    CPUID_INTELBRANDSTRING,
    CPUID_INTELBRANDSTRINGMORE,
    CPUID_INTELBRANDSTRINGEND                   = 0x80000004,
    CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_AMD      = 0x8000000D // Jerarquía y topología de caché (para MAD)
} cpuid_requests;

typedef enum CPUID_Extended_Features_page { // se usa junto a CPUID_Extended_Features, donde este se pasa a EAX, y a ECX un CPUID_Extended_Features_page
    CPUID_Extended_Features_page_1,
    CPUID_Extended_Features_page_2,
    CPUID_Extended_Features_page_3
} CPUID_Extended_Features_page; 
typedef enum CPUID_XSAVE_features_and_state_components_page { 
    CPUID_XSAVE_features_and_state_components_page_1,
    CPUID_XSAVE_features_and_state_components_page_2,
    CPUID_XSAVE_features_and_state_components_page_3
                                                   // se usa junto a CPUID_XSAVE_FEATURES_AND_STATE_COMPONENTS, 
} CPUID_XSAVE_features_and_state_components_page;  // donde este se pasa a EAX, y a ECX un CPUID_XSAVE_features_and_state_components_page
typedef enum CPUID_SGX_capabilities_page { 
    CPUID_SGX_capabilities_page_1,
    CPUID_SGX_capabilities_page_2,
    CPUID_SGX_capabilities_page_3
                                                   // se usa junto a CPUID_SGX_CAPATIBLES, 
} CPUID_SGX_capabilities_page;  // donde este se pasa a EAX, y a ECX un CPUID_SGX_CAPATIBLES
typedef enum CPUID_Processor_Trace_page { 
    CPUID_Processor_Trace_page_1,
    CPUID_Processor_Trace_page_2
                                                   // se usa junto a CPUID_PROCESSOR_TRACE, 
} CPUID_Processor_Trace_page;  // donde este se pasa a EAX, y a ECX un CPUID_PROCESSOR_TRACE
typedef enum CPUID_AVX10_Features_page { 
    CPUID_AVX10_Features_page_1,
    CPUID_AVX10_Features_page_2
                                                   // se usa junto a CPUID_AVX10_FEATURES, 
} CPUID_AVX10_Features_page;  // donde este se pasa a EAX, y a ECX un CPUID_AVX10_FEATURES


#pragma pack(push)
typedef struct manufacturer_ID
{
    uint32_t EBX;
    uint32_t EDX;
    uint32_t ECX;
    uint8_t final_string; // \0
} manufacturer_ID;

typedef struct Processor_Info_and_Feature_Bits {
    uint32_t Stepping_ID        :4; // 3  - 0
    uint32_t Model              :4; // 7  - 4
    uint32_t Family_ID          :4; // 11 - 8
    uint32_t Processor_Type     :2; // 13 - 12
    uint32_t Reserved2          :2; // 15 - 14
    uint32_t Extended_Model_ID  :4; // 19 - 16
    uint32_t Extended_Family_ID :8; // 27 - 20
    uint32_t Reserved1          :4; // 31 - 28
} Processor_Info_and_Feature_Bits;
typedef struct Additional_Information_Feature_Bits { // para CPUID con EAX=1; esta estructura esta en EBX
    uint32_t Brand_Index        :8; // 7  - 0   Descripción: El Brand Index es un identificador único del modelo del procesador.
                                    //          Validez: Siempre válido.
                                    //          Uso: Se utiliza para identificar y diferenciar entre modelos de procesadores de una misma familia.
    uint32_t CLFLUSH            :8; // 15 - 8   tamaño de línea (Valor * 8 = tamaño de línea de caché en bytes)
                                    //          Descripción: Tamaño de la línea de caché para la instrucción CLFLUSH.
                                    //          Validez: Este campo es válido si el bit 19 de CPUID.01.EDX (CLFSH) está establecido en 1.
                                    //          Cálculo: El valor en estos bits multiplicado por 8 da el tamaño de la línea de caché en bytes.
    uint32_t Max_ID_addressable :8; // 23 - 16  Número máximo de ID direccionables para procesadores lógicos en este paquete físico;
                                    //          El entero de potencia de 2 más cercano que no sea menor que este valor es el número 
                                    //          de ID de APIC iniciales únicos reservados para direccionar diferentes procesadores lógicos en un paquete físico.
                                    //          Uso anterior: Número de procesadores lógicos por procesador físico; dos para el 
                                    //          procesador Pentium 4 con tecnología Hyper-Threading.
                                    // En CPU con más de 128 procesadores lógicos en un solo paquete (por ejemplo, 
                                    // Intel Xeon Phi 7290[41] y AMD Threadripper Pro 7995WX[42]), el valor en el bit 23:16 se 
                                    // establece en un valor sin potencia de 2.
                                    //
                                    // Descripcion:
                                    // Concepto Básico;
                                    // Los procesadores modernos pueden ejecutar múltiples hilos de ejecución al mismo tiempo. 
                                    // Esto es posible gracias a tecnologías como el Hyper-Threading de Intel, donde un solo procesador 
                                    // físico puede actuar como si fuera varios procesadores lógicos. Cada uno de estos procesadores lógicos 
                                    // puede ejecutar su propio hilo de ejecución.
                                    // Número Máximo de IDs Direccionables para Procesadores Lógicos
                                    // Imagina que tienes una fábrica con varias líneas de producción. Cada línea de producción puede trabajar en 
                                    // una tarea diferente al mismo tiempo. En nuestro ejemplo, cada línea de producción es como un procesador lógico
                                    // dentro de un procesador físico.
                                    // El "Número máximo de IDs direccionables para procesadores lógicos" es como el número máximo de líneas de 
                                    // producción que puede tener tu fábrica. Este número indica cuántos procesadores lógicos diferentes puede 
                                    // gestionar tu procesador físico.
                                    // El "Número máximo de IDs direccionables para procesadores lógicos" es como el número máximo de líneas de 
                                    // producción que puede tener tu fábrica. Este número indica cuántos procesadores lógicos diferentes puede 
                                    // gestionar tu procesador físico.
                                    // Por Qué es Importante?
                                    // Identificación Única:
                                    // Cada procesador lógico necesita una identificación única para que el sistema operativo y las aplicaciones 
                                    // puedan comunicarse con él de manera efectiva. Es como tener un número de identificación único para cada línea de producción.
                                    // Optimización del Rendimiento:
                                    // Sabiendo cuántos procesadores lógicos puede manejar el procesador físico, el sistema operativo puede optimizar cómo 
                                    // distribuir las tareas entre ellos. Es similar a asignar diferentes tareas a cada línea de producción para aprovechar 
                                    // al máximo la capacidad de la fábrica.
                                    // Configuración del Sistema:
                                    // Esta información ayuda a configurar correctamente el sistema, especialmente en entornos de servidores o estaciones de 
                                    // trabajo donde es crucial aprovechar al máximo los recursos del hardware.
                                    // Ejemplo Simplificado
                                    // Supongamos que tienes un procesador físico que, gracias a Hyper-Threading, puede manejar hasta 8 procesadores lógicos. 
                                    // Esto significa que el sistema operativo puede ver y utilizar hasta 8 procesadores como si fueran independientes, aunque 
                                    // físicamente solo haya uno.
                                    // Procesador Físico: 1
                                    // Procesadores Lógicos: 8
                                    // Número máximo de IDs direccionables: 8
                                    // Este número máximo (8 en nuestro ejemplo) es crucial para entender cuántos hilos de ejecución puedes correr 
                                    // simultáneamente en tu sistema.
                                    // En Resumen
                                    // El "Número máximo de IDs direccionables para procesadores lógicos" es un valor que indica cuántos procesadores 
                                    // lógicos puede manejar un procesador físico. Esto permite que el sistema operativo y las aplicaciones optimicen el uso del 
                                    // hardware, asegurando que las tareas se distribuyan eficientemente entre los distintos procesadores lógicos disponibles.




    uint32_t Local_APIC_ID      :8; //  El ID de APIC local también se puede identificar a través de la hoja cpuid 0Bh 
                                    //  ( CPUID.0Bh.EDX[x2APIC-ID] ). En CPU con más de 256 procesadores lógicos en un paquete 
                                    //  (por ejemplo, Xeon Phi 7290), se debe utilizar la hoja 0Bh porque el ID APIC no cabe en 8 bits.
                                    //  Descripción: ID APIC local del procesador lógico que está ejecutando la instrucción. APIC 
                                    //  (Advanced Programmable Interrupt Controller) es un componente que maneja interrupciones en sistemas multiprocesador
                                    //  El "Local APIC ID" es importante porque permite identificar de forma única cada procesador lógico. En sistemas con 
                                    //  múltiples procesadores, puede haber conflictos o problemas si dos procesadores tienen el mismo identificador de APIC local.
                                    //  El "Local APIC ID" se utiliza en el manejo de interrupciones y en la coordinación de tareas entre los diferentes 
                                    //  procesadores en un sistema multiprocesador. Cada procesador utiliza su identificador de APIC local para comunicarse 
                                    //  con el APIC local y coordinar las interrupciones y otras actividades del sistema.
                                    //  Validez: Válido para procesadores Pentium 4 y posteriores.
                                    // Uso: El APIC-ID inicial se utiliza para identificar el procesador lógico en ejecución.
                                    // resumen: es un identificador único asignado a cada procesador lógico en un sistema multiprocesador 
                                    // y se utiliza para coordinar las actividades del sistema
                                
} Additional_Information_Feature_Bits;


typedef struct cpuid_feat_ecx {
    uint32_t SSE3       :1; // 00
    uint32_t PCLMUL     :1; // 01
    uint32_t DTES64     :1; // 02
    uint32_t MONITOR    :1; // 03
    uint32_t DS_CPL     :1; // 04
    uint32_t VMX        :1; // 05
    uint32_t SMX        :1; // 06
    uint32_t EST        :1; // 07
    uint32_t TM2        :1; // 08
    uint32_t SSSE3      :1; // 09
    uint32_t CID        :1; // 10
    uint32_t SDBG       :1; // 11
    uint32_t FMA        :1; // 12
    uint32_t CX16       :1; // 13
    uint32_t XTPR       :1; // 14
    uint32_t PDCM       :1; // 15
    uint32_t RESERVADO  :1; // 16
    uint32_t PCID       :1; // 17
    uint32_t DCA        :1; // 18
    uint32_t SSE4_1     :1; // 19
    uint32_t SSE4_2     :1; // 20
    uint32_t X2APIC     :1; // 21
    uint32_t MOVBE      :1; // 22
    uint32_t POPCNT     :1; // 23
    uint32_t TSC        :1; // 24
    uint32_t AES        :1; // 25
    uint32_t XSAVE      :1; // 26
    uint32_t OSXSAVE    :1; // 27
    uint32_t AVX        :1; // 28
    uint32_t F16C       :1; // 29
    uint32_t RDRAND     :1; // 30
    uint32_t HYPERVISOR :1; // 31
} cpuid_feat_ecx;

typedef struct cpuid_feat_edx {
    uint32_t FPU        :1; // 00
    uint32_t VME        :1; // 01
    uint32_t DE         :1; // 02
    uint32_t PSE        :1; // 03
    uint32_t TSC        :1; // 04
    uint32_t MSR        :1; // 05
    uint32_t PAE        :1; // 06
    uint32_t MCE        :1; // 07
    uint32_t CX8        :1; // 08
    uint32_t APIC       :1; // 09
    uint32_t RESERVADO  :1; // 10
    uint32_t SEP        :1; // 11
    uint32_t MTRR       :1; // 12
    uint32_t PGE        :1; // 13
    uint32_t MCA        :1; // 14
    uint32_t CMOV       :1; // 15
    uint32_t PAT        :1; // 16
    uint32_t PSE36      :1; // 17
    uint32_t PSN        :1; // 18
    uint32_t CLFLUSH    :1; // 19
    uint32_t NX         :1; // 20
    uint32_t DS         :1; // 21
    uint32_t ACPI       :1; // 22
    uint32_t MMX        :1; // 23
    uint32_t FXSR       :1; // 24
    uint32_t SSE        :1; // 25
    uint32_t SSE2       :1; // 26
    uint32_t SS         :1; // 27
    uint32_t HTT        :1; // 28
    uint32_t TM         :1; // 29
    uint32_t IA64       :1; // 30
    uint32_t PBE        :1; // 31
} cpuid_feat_edx;

#pragma pack(pop)

typedef enum Processor_Family_ID_Amd {
    Amd486         = 0x4,
    Amd5x86        = Amd486,
    Elan_SC4xx_5xx = Amd486,

    K5,
    K6             = K5,

    K7_Athlon,
    Athlon_XP      = K7_Athlon,

    K8_Hammer      = 0x0f,             // Athlon 64
    K10_Phenom     = 0x10,
    Turion_X2,
    Llano,

    Bobcat         = 0x14,
    Bulldozer,
    Piledriver     = Bulldozer,
    Steamroller    = Bulldozer,
    Excavator      = Bulldozer,

    Jaguar,
    Puma           = Jaguar,

    Zen1,
    Zen2           = Zen1, 
    Hygon_Dhyana,
    Zen3,
    Zen4           = Zen3,
    Zen5
} Processor_Family_ID_Amd;

typedef enum Processor_Family_ID_Intel {
    // El procesador i386 no admite la instrucción CPUID; sin embargo, devuelve 
    // el ID de familia 3h en el valor de reinicio de EDX.
    i486 = 0x4,

    Pentium,
    Pentium_MMX            = Pentium,
    Quark_X1000            = Pentium,
        
    Pentium_Pro,        
    Pentium_II             = Pentium_Pro,
    Pentium_III            = Pentium_Pro,
    Pentium_M              = Pentium_Pro,
    Intel_Core             = Pentium_Pro, // todas las variantes del intel core mismo ID
    Intel_Atom             = Pentium_Pro, // todas las variantes mismo ID
    Intel_Xeon             = Pentium_Pro, // excepto las variantes de NetBurst
    Intel_Xeon_Phi         = Pentium_Pro, // excepto KNC

    Itanium,                              // en modo IA-32

    // Se ha informado que el ID de familia 8h se ha evitado deliberadamente para 
    // la familia de procesadores Pentium 4 debido a la incompatibilidad con Windows NT 4.0

    Xeon_Phi_KnightsCorner = 0x0B,        // https://es.wikipedia.org/wiki/Intel_MIC
    Pentium_4_NetBurst     = 0x0F,
    Itanium_2              = 0x11,        // en modo IA-32

} Processor_Family_ID_Intel;

// Processor Type values:
#define OEM_Processor                 0b00
#define Intel_Overdrive_Processor     0b01
#define Dual_processor                0b10 // solo para procesadores P5 Pentium
#define Processor_Type_value_reserved 0b11 


// https://wiki.osdev.org/CPUID
// Vendor strings from CPUs.
#define CPUID_VENDOR_AMD                   "AuthenticAMD"
#define CPUID_VENDOR_AMD_OLD               "AMDisbetter!" // Early engineering samples of AMD K5 processor
#define CPUID_VENDOR_INTEL                 "GenuineIntel"
#define CPUID_VENDOR_VIA                   "VIA VIA VIA "
#define CPUID_VENDOR_TRANSMETA             "GenuineTMx86"
#define CPUID_VENDOR_TRANSMETA_OLD         "TransmetaCPU"
#define CPUID_VENDOR_CYRIX                 "CyrixInstead"
#define CPUID_VENDOR_CENTAUR               "CentaurHauls"
#define CPUID_VENDOR_NEXGEN                "NexGenDriven"
#define CPUID_VENDOR_UMC                   "UMC UMC UMC "
#define CPUID_VENDOR_SIS                   "SiS SiS SiS "
#define CPUID_VENDOR_NSC                   "Geode by NSC"
#define CPUID_VENDOR_RISE                  "RiseRiseRise"
#define CPUID_VENDOR_VORTEX                "Vortex86 SoC"
#define CPUID_VENDOR_AO486                 "MiSTer AO486"
#define CPUID_VENDOR_AO486_OLD             "GenuineAO486"
#define CPUID_VENDOR_ZHAOXIN               "  Shanghai  "
#define CPUID_VENDOR_HYGON                 "HygonGenuine"
#define CPUID_VENDOR_ELBRUS                "E2K MACHINE "
 
// https://www.qemu.org/docs/master/system/i386/hyperv.html
// CPUID EAX=40000x00h: 12-character Hypervisor ID string in EBX,ECX,EDX
#define CPUID_VENDOR_HYPERV                "Microsoft Hv" // https://learn.microsoft.com/en-us/virtualization/hyper-v-on-windows/tlfs/feature-discovery
#define CPUID_VENDOR_KVM1                  " KVMKVMKVM  "
#define CPUID_VENDOR_KVM2                  "KVMKVMKVM\0\0\0"
#define CPUID_VENDOR_KVM3                  "Linux KVM Hv"
#define CPUID_VENDOR_BHYVE1                "bhyve bhyve "
#define CPUID_VENDOR_BHYVE2                "BHyVE BHyVE "
#define CPUID_VENDOR_XEN                   "XenVMMXenVMM"
#define CPUID_VENDOR_QEMU                  "TCGTCGTCGTCG"
#define CPUID_VENDOR_PARALLELS_ALT         " lrpepyh vr " // Sometimes Parallels incorrectly encodes "prl hyperv" as "lrpepyh vr" due to an endianness mismatch.s
#define CPUID_VENDOR_PARALLELS             " prl hyperv "
#define CPUID_VENDOR_VMWARE                "VMwareVMware"
#define CPUID_VENDOR_PPROJECT_ACRN         "ACRNACRNACRN"
#define CPUID_VENDOR_VIRTUALBOX            "VBoxVBoxVBox"
#define CPUID_VENDOR_QNX1                  " QNXQVMBSQG "
#define CPUID_VENDOR_QNX2                  "QXNQSBMV"      // El método de detección del hipervisor QNX proporcionado en la documentación oficial de QNX[117] 
                                                           //verifica solo los primeros 8 caracteres de la cadena, como se proporciona en EBX y ECX (incluido 
                                                           // un intercambio de endianness ); EDX se ignora y puede tomar cualquier valor.
#define CPUID_VENDOR_NETBSD_NVMM           "___ NVMM ___"
#define CPUID_VENDOR_OPENBSD_VMM           "OpenBSDVMM58"
#define CPUID_VENDOR_INTEL_HAXM            "HAXMHAXMHAXM"
#define CPUID_VENDOR_INTEL_KGT             "EVMMEVMMEVMM"
#define CPUID_VENDOR_UNISYS_S_PAR          "UnisysSpar64"
#define CPUID_VENDOR_LOCKHEED_MARTIN_LMHS  "SRESRESRESRE"

/* 
    caracteristicas de la cpu:
    Cuando se llama con EAX = 1 (CPUID_GETFEATURES), CPUID devuelve un campo de bits 
    en EDX que contiene los siguientes valores. Tenga en cuenta que diferentes marcas 
    de CPU pueden haberles dado diferentes significados. Los procesadores recientes también 
    utilizan ECX para funciones (que forman un conjunto diferente), con lo que debes tener mucho 
    cuidado ya que algunas CPU antiguas devuelven información falsa en este registro.
*/
typedef enum CPUID_FEAT {
    // para CPUID EAX=1: Feature Information in ECX
    CPUID_FEAT_ECX_SSE3         = 1 << 0, 
    CPUID_FEAT_ECX_PCLMUL       = 1 << 1,
    CPUID_FEAT_ECX_DTES64       = 1 << 2,
    CPUID_FEAT_ECX_MONITOR      = 1 << 3,  
    CPUID_FEAT_ECX_DS_CPL       = 1 << 4,  
    CPUID_FEAT_ECX_VMX          = 1 << 5,  
    CPUID_FEAT_ECX_SMX          = 1 << 6,  
    CPUID_FEAT_ECX_EST          = 1 << 7,  
    CPUID_FEAT_ECX_TM2          = 1 << 8,  
    CPUID_FEAT_ECX_SSSE3        = 1 << 9,  
    CPUID_FEAT_ECX_CID          = 1 << 10,
    CPUID_FEAT_ECX_SDBG         = 1 << 11,
    CPUID_FEAT_ECX_FMA          = 1 << 12,
    CPUID_FEAT_ECX_CX16         = 1 << 13, 
    CPUID_FEAT_ECX_XTPR         = 1 << 14, 
    CPUID_FEAT_ECX_PDCM         = 1 << 15, 
    CPUID_FEAT_ECX_RESERVADO    = 1 << 16,  
    CPUID_FEAT_ECX_PCID         = 1 << 17, 
    CPUID_FEAT_ECX_DCA          = 1 << 18, 
    CPUID_FEAT_ECX_SSE4_1       = 1 << 19, 
    CPUID_FEAT_ECX_SSE4_2       = 1 << 20, 
    CPUID_FEAT_ECX_X2APIC       = 1 << 21, 
    CPUID_FEAT_ECX_MOVBE        = 1 << 22, 
    CPUID_FEAT_ECX_POPCNT       = 1 << 23, 
    CPUID_FEAT_ECX_TSC          = 1 << 24, 
    CPUID_FEAT_ECX_AES          = 1 << 25, 
    CPUID_FEAT_ECX_XSAVE        = 1 << 26, 
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 27, 
    CPUID_FEAT_ECX_AVX          = 1 << 28,
    CPUID_FEAT_ECX_F16C         = 1 << 29,
    CPUID_FEAT_ECX_RDRAND       = 1 << 30,
    CPUID_FEAT_ECX_HYPERVISOR   = 1 << 31,
 
    // para CPUID EAX=1: Feature Information in EDX
    CPUID_FEAT_EDX_FPU          = 1 << 0,  
    CPUID_FEAT_EDX_VME          = 1 << 1,  
    CPUID_FEAT_EDX_DE           = 1 << 2,  
    CPUID_FEAT_EDX_PSE          = 1 << 3,  
    CPUID_FEAT_EDX_TSC          = 1 << 4,  
    CPUID_FEAT_EDX_MSR          = 1 << 5,  
    CPUID_FEAT_EDX_PAE          = 1 << 6,  
    CPUID_FEAT_EDX_MCE          = 1 << 7,  
    CPUID_FEAT_EDX_CX8          = 1 << 8,  
    CPUID_FEAT_EDX_APIC         = 1 << 9,  
    CPUID_FEAT_EDX_RESERVADO    = 1 << 10,  
    CPUID_FEAT_EDX_SEP          = 1 << 11, 
    CPUID_FEAT_EDX_MTRR         = 1 << 12, 
    CPUID_FEAT_EDX_PGE          = 1 << 13, 
    CPUID_FEAT_EDX_MCA          = 1 << 14, 
    CPUID_FEAT_EDX_CMOV         = 1 << 15, 
    CPUID_FEAT_EDX_PAT          = 1 << 16, 
    CPUID_FEAT_EDX_PSE36        = 1 << 17, 
    CPUID_FEAT_EDX_PSN          = 1 << 18, 
    CPUID_FEAT_EDX_CLFLUSH      = 1 << 19, 
    CPUID_FEAT_EDX_NX           = 1 << 20,  
    CPUID_FEAT_EDX_DS           = 1 << 21, 
    CPUID_FEAT_EDX_ACPI         = 1 << 22, 
    CPUID_FEAT_EDX_MMX          = 1 << 23, 
    CPUID_FEAT_EDX_FXSR         = 1 << 24, 
    CPUID_FEAT_EDX_SSE          = 1 << 25, 
    CPUID_FEAT_EDX_SSE2         = 1 << 26, 
    CPUID_FEAT_EDX_SS           = 1 << 27, 
    CPUID_FEAT_EDX_HTT          = 1 << 28, 
    CPUID_FEAT_EDX_TM           = 1 << 29, 
    CPUID_FEAT_EDX_IA64         = 1 << 30,
    CPUID_FEAT_EDX_PBE          = 1 << 31
} CPUID_FEAT;

#include "cpuid.c"
#endif