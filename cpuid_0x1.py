from cpuid_x86 import Cpuid, Register, CPUID_GETFEATURES
from typing import Optional, List, Tuple

class Processor_Info_and_Feature_Bits():
    """ 
    >>> import cpu_id_x86
    >>> a = cpu_id_x86.Processor_Info_and_Feature_Bits()
    >>> dir(a)
    [
        di'All_Model', 'CPUID_signature', 'Extended_Family_ID', 'Extended_Model_ID', 
        'Family_ID', 'Model', 'Processor_Type', 'Reserved1', 'Reserved2', 'Stepping_ID',
        '__class__', '__delattr__', '__dict__', '__dir__', '__doc__', '__eq__', '__format__', 
        '__ge__', '__getattribute__', '__getstate__', '__gt__', '__hash__', '__init__', 
        '__init_subclass__', '__le__', '__lt__', '__module__', '__ne__', '__new__', 
        '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', '__sizeof__', '__str__', 
        '__subclasshook__', '__weakref__', 'additional_info', 'extensions', 
        'get_model_number', 'print_vals'
    ]
    >>> print(a.extensions)
    CPU Features: FPU, VME, DE, PSE, TSC, MSR, MCE, CX8, APIC, RESERVADO, SEP, MTRR, PGE, 
    MCA, CMOV, PSE36, CLFLUSH, NX, DS, ACPI, MMX, FXSR, SSE, SSE2, SS, HTT, TM, IA64, SSE3, 
    PCLMUL, DTES64, MONITOR, DS_CPL, VMX, SMX, EST, TM2, SSSE3, SDBG, FMA, CX16, XTPR, PDCM,
    RESERVADO, PCID, SSE4_1, X2APIC, MOVBE, POPCNT, TSC, AES, XSAVE, OSXSAVE, AVX, F16C, HYPERVISOR
    >>> help(a.additional_info)
    >>> print(a.additional_info)
    Brand Index: 0
    CLFLUSH Line Size: 64 bytes
    Max Addressable IDs: 128
    Local APIC ID: 68

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
    """
    def __init__(self, eax:Optional[int] = None):
        """
            Obtiene la informacion de la cpu y la guarda en la clase.
            Esta informacion se obtiene desde la clase cpuid_x86.Cpuid
            si el argumento eax es None.
            En caso contrario, se usa el valor eax de entrada para obtener
            los datos.
            
            En algunas procesadores. el campo self.Processor_Type esta reservado
        Args:
            eax (int, optional): valor eax desde el que
            obtener la informacion, en caso de ser None, se obtendra el valor
            con las clase Cpuid. Defaults to None.
        """
        if eax == None:
            my_request_cpuid = Cpuid(Register(
                CPUID_GETFEATURES,
                0, 0, 0
            ))
            # acceder al registro interno de la peticion de cpuid
            eax = my_request_cpuid.reg.eax
            
        self.Stepping_ID        :int = (eax & 0b00000000000000000000000000001111)
        self.Model              :int = (eax & 0b00000000000000000000000011110000) >> 4
        self.Family_ID          :int = (eax & 0b00000000000000000000111100000000) >> 8
        self.Processor_Type     :int = (eax & 0b00000000000000000011000000000000) >> 12
        self.Reserved2          :int = (eax & 0b00000000000000001100000000000000) >> 14
        self.Extended_Model_ID  :int = (eax & 0b00000000000011110000000000000000) >> 16
        self.Extended_Family_ID :int = (eax & 0b00001111111100000000000000000000) >> 20
        self.Reserved1          :int = (eax & 0b11110000000000000000000000000000) >> 28
        
        self.All_Model          :int = self.get_model_number(self.Family_ID, self.Model, self.Extended_Model_ID)
        
        """ Ejemplo:
        CPUID signature: 0x0671 (0671h)
        Family:	           0x06 (06h)
        Model:	           0x07 (07h)
        Stepping:          0x01 (01h)
        """ 
        self.CPUID_signature    :int = (self.Family_ID << 8) + \
                                       (self.Model     << 4) + \
                                       (self.Stepping_ID)
        
        self.additional_info = Additional_Information_Feature_Bits(my_request_cpuid.reg.ebx)
        self.extensions      = Information_Feature_Bits(ecx=my_request_cpuid.reg.ecx, edx=my_request_cpuid.reg.edx)
        
    def get_model_number(
            self, 
            Family_ID         :Optional[int] = None, 
            Model             :Optional[int] = None, 
            Extended_Model_ID :Optional[int] = None
        ) -> int:
        #((Family_ID == 0x06) || (Family_ID == 0x0f)) ? ((Extended_Model_ID << 4) + Model_ID) : Model_ID
        Family_ID          = Family_ID          if Family_ID         is not None else getattr(self, 'Family_ID', 0)
        Model              = Model              if Model             is not None else getattr(self, 'Model', 0)
        Extended_Model_ID  = Extended_Model_ID  if Extended_Model_ID is not None else getattr(self, 'Extended_Model_ID', 0)
        # if (Family_ID == None): Family_ID = self.Family_ID
        # if (Model == None): Model = self.Model
        # if (Extended_Model_ID == None): Extended_Model_ID = self.Extended_Model_ID
        
        return (
            (Extended_Model_ID << 4) + Model)               \
                if (Family_ID == 0x06 or Family_ID == 0x0f) \
                else Model

    
    def print_vals(self) -> None:
        print(
            f"Reserved1           :4  = {hex(self.Reserved1)}\n"          +
            f"Extended_Family_ID  :8  = {hex(self.Extended_Family_ID)}\n" +
            f"Extended_Model_ID   :4  = {hex(self.Extended_Model_ID)}\n"  +
            f"Reserved2           :2  = {hex(self.Reserved2)}\n"          +
            f"Processor_Type      :2  = {hex(self.Processor_Type)}\n"     +
            f"Family_ID           :4  = {hex(self.Family_ID)}\n"          +
            f"Model               :4  = {hex(self.Model)}\n"              +
            f"Stepping_ID         :4  = {hex(self.Stepping_ID)}\n"        +
            f"All_Model               = {hex(self.All_Model)}\n"          +
            f"CPUID signature         = {hex(self.CPUID_signature)}\n"    
        )

class Information_Feature_Bits():
    """
    typedef struct cpuid_feat_ecx {
        uint32_t SSE3       :1; // 00
        uint32_t PCLMUL     :1; // 01
        uint32_t DTES64     :1; // 02
        uint32_t MONITOR    :1; // 03
        uint32_t DS_CPL     :1; // 04 CPL-qualified debug store
        uint32_t VMX        :1; // 05 VMX technology
        uint32_t SMX        :1; // 06
        uint32_t EST        :1; // 07 Enhanced SpeedStep technology
        uint32_t TM2        :1; // 08 Thermal Monitor 2 supported
        uint32_t SSSE3      :1; // 09
        uint32_t CID        :1; // 10 L1 context ID supported
        uint32_t SDBG       :1; // 11
        uint32_t FMA        :1; // 12
        uint32_t CX16       :1; // 13 CompareAndExchange16B supported
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
        uint32_t FPU        :1; // 00 FPU87 on chip
        uint32_t VME        :1; // 01 Virtual 8086 extensions
        uint32_t DE         :1; // 02 Debugging extensions
        uint32_t PSE        :1; // 03 Page Size extensions
        uint32_t TSC        :1; // 04 TimeStamp Counter
        uint32_t MSR        :1; // 05 RDMSR/WRMSR support
        uint32_t PAE        :1; // 06 Physical Address Extension
        uint32_t MCE        :1; // 07 Machine Check Exception
        uint32_t CX8        :1; // 08 CMXCHG8B
        uint32_t APIC       :1; // 09 APIC on-chip
        uint32_t RESERVADO  :1; // 10 
        uint32_t SEP        :1; // 11 SYSENTER/SYSEXIT
        uint32_t MTRR       :1; // 12 Memory Type Range Registers
        uint32_t PGE        :1; // 13 Global PTE bit
        uint32_t MCA        :1; // 14 Machine Check Architecture
        uint32_t CMOV       :1; // 15 CMOV: Conditional move/compare instruction
        uint32_t PAT        :1; // 16 Page attribute table
        uint32_t PSE36      :1; // 17 PSE36 Size extensions
        uint32_t PSN        :1; // 18 Processor serial number
        uint32_t CLFLUSH    :1; // 19 CFLUSH instruction
        uint32_t NX         :1; // 20 Bit NX
        uint32_t DS         :1; // 21 Debug Store
        uint32_t ACPI       :1; // 22 ACPI
        uint32_t MMX        :1; // 23 MMX
        uint32_t FXSR       :1; // 24 FXSAVE/FXRESTORE
        uint32_t SSE        :1; // 25 SSE extensions
        uint32_t SSE2       :1; // 26 SSE2 extensions
        uint32_t SS         :1; // 27 Self-Snoop
        uint32_t HTT        :1; // 28 Hyperthreading
        uint32_t TM         :1; // 29 Thermal Monitor
        uint32_t IA64       :1; // 30 
        uint32_t PBE        :1; // 31 Pending Break Enable
    } cpuid_feat_edx;

        [
            'ACPI', 'AES', 'APIC', 'AVX', 'CID', 'CLFLUSH', 'CMOV', 'CX16', 'CX8', 
            'DCA', 'DE', 'DS', 'DS_CPL', 'DTES64', 'EST', 'F16C', 'FMA', 'FPU', 
            'FXSR', 'HTT', 'HYPERVISOR', 'IA64', 'MCA', 'MCE', 'MMX', 'MONITOR', 
            'MOVBE', 'MSR', 'MTRR', 'NX', 'OSXSAVE', 'PAE', 'PAT', 'PBE', 'PCID', 
            'PCLMUL', 'PDCM', 'PGE', 'POPCNT', 'PSE', 'PSE36', 'PSN', 'RDRAND', 
            'RESERVADO', 'SDBG', 'SEP', 'SMX', 'SS', 'SSE', 'SSE2', 'SSE3', 
            'SSE4_1', 'SSE4_2', 'SSSE3', 'TM', 'TM2', 'TSC', 'VME', 'VMX', 
            'X2APIC', 'XSAVE', 'XTPR', '__class__', '__delattr__', '__dict__', 
            '__dir__', '__doc__', '__eq__', '__format__', '__ge__', 
            '__getattribute__', '__getstate__', '__gt__', '__hash__', '__init__', 
            '__init_subclass__', '__le__', '__lt__', '__module__', '__ne__', 
            '__new__', '__reduce__', '__reduce_ex__', '__repr__', '__setattr__', 
            '__sizeof__', '__str__', '__subclasshook__', '__weakref__', '_features', 
            '_set_features', 'has_all_features', 'has_any_feature', 'has_feature', 'print_vals'
        ]
    """ 
    
    def _set_features(self, 
            val   :int, 
            _list :List[Tuple[str, int]], 
            index :Optional[int] = 0
        ):
        """
            Se usa para crear los atributos de la clase con la informacion de la tecnologias que
            soporta la CPU.

        Args:
            val (int): Valor del registro con el que obtener la informacion. ECX o EBX
            _list (List[Tuple[str, int]]): Matriz de 2 x X elementos. Donde cada elemento X
                es una tecnologia de la CPU, que se indica con una tupla (Nombre_atributo, bit_mask).
                El bit_mask indica que valor identifica esta tecnologia.
            index (Optional[int], optional): Indica a que posicion de la matriz acceder,
                Index = 0 para acceder a las tecnologias de ECX, Index = 1 para acceder
                a las tecnologias del registro EDX. Defaults to 0.
        """
        for feature, bit in _list[index]:
            setattr(self, feature, bool(val & (1 << bit)))
            # setattr(clase, Nombre_atributo, valor atributo)
                
    def __init__(
            self, 
            ecx:Optional[int] = None,
            edx:Optional[int] = None
        ):
        """
            Se usa para obtener informacion acerca de las tecnologias que la CPU
            soporta y brinda.

        Args:
            edx (int, optional): parametro EDX a obtener mediante la clase cpuid_x86.Cpuid.
            En caso de ser None se accedera a llamar a None, en caso de ser un valor entero,
            se obtendra las tecnologias disponibles apartir de alli.
            Defaults to None.
            
            ecx (int, optional): _description_. Defaults to None.
        """
        if edx is None or ecx is None: 
            my_request_cpuid = Cpuid(Register(CPUID_GETFEATURES, 0, 0, 0 ))
            # Obtener los registros de la clase Reg interna de la llamda Cpuid si se declaro como 
            # None en los argumentos
            edx = edx if edx is not None else getattr(my_request_cpuid.reg, 'edx', 0)
            ecx = ecx if ecx is not None else getattr(my_request_cpuid.reg, 'ecx', 0)

        # edx
        self._features = [
            [
                ('FPU',   0), ('VME',    1), ('DE',   2), ('PSE',      3), ('TSC',        4), ('MSR', 5),
                ('PAE',   6), ('MCE',    7), ('CX8',  8), ('APIC',     9), ('RESERVADO', 10),
                ('SEP',  11), ('MTRR',  12), ('PGE', 13), ('MCA',     14), ('CMOV',      15),
                ('PAT',  16), ('PSE36', 17), ('PSN', 18), ('CLFLUSH', 19), ('NX',        20),
                ('DS',   21), ('ACPI',  22), ('MMX', 23), ('FXSR',    24), ('SSE',       25),
                ('SSE2', 26), ('SS',    27), ('HTT', 28), ('TM',      29), ('IA64',      30), ('PBE', 31)
            ],
            [
                ("SSE3",    0), ("PCLMUL",      1), ("DTES64",  2), ("MONITOR",  3), ("DS_CPL",     4), ("VMX",     5), 
                ("SMX",     6), ("EST",         7), ("TM2",     8), ("SSSE3",    9), ("CID",       10), ("SDBG",   11), 
                ("FMA",    12), ("CX16",       13), ("XTPR",   14), ("PDCM",    15), ("RESERVADO", 16), ("PCID",   17),
                ("DCA",    18), ("SSE4_1",     19), ("SSE4_2", 20), ("X2APIC",  21), ("MOVBE",     22), ("POPCNT", 23), 
                ("TSC",    24), ("AES",        25), ("XSAVE",  26), ("OSXSAVE", 27), ("AVX",       28), ("F16C",   29),
                ("RDRAND", 30), ("HYPERVISOR", 31),
            ]
        ]
        self._set_features(ecx, self._features, index = 0)
        self._set_features(edx, self._features, index = 1)
        

    def __str__(self):
        features = []
        for feature_list in self._features:
            for feature, _ in feature_list:
                if getattr(self, feature, False):
                    features.append(feature)
        return f"CPU Features: {', '.join(features)}"

        
    def has_feature(self, feature:str) -> bool:
        """
        Verifica si una capacidad esta activa en la CPU, por ejemplo asi se puede comprobar si se 
        tiene SSE3:
        
        cpu_info = Information_Feature_Bits()
        if cpu_info.has_feature('SSE3'):
            print("Esta CPU soporta SSE3")
        else:
            print("Esta CPU no soporta SSE3")

        Args:
            feature (str): Tecnologia que deseas comprobar si esta presente.

        Returns:
            bool: Si existe la tecnologia especificada el valor devuelve True, en caso contrario, False.
        """
        return getattr(self, feature, False)

    def has_all_features(self, *features:str) -> bool:
        """
            La funcion verifica si la caracteristica o carcateristicas especificadas 
            estan TODAS presentes. En caso de que alguna no lo este, la funcion devolvera False.
            En caso contrario, devolvera True:
            
            Si la CPU soporta SSE, SSE2 y SSE3:
                a.has_all_features("SSE", "SSE2", "SSE3") == True
                # en el caso de que por ejemplo no tenga SSE3, se devolvera False
        """
        return all(self.has_feature(f) for f in features)

    def has_any_feature(self, *features:str) -> bool:
        """
            La funcion verifica si la caracteristica o carcateristicas especificadas estan presentes.
            Con que se de una de las ingreasadas, el metodo devolvera True.
            
            Ejemplo con extension PAT en la CPU:
                a.has_any_feature("PAT") = True
                # Si la CPU no tiene PAT, devolvera False
                
            Ejemplo con extension SSE, SSE2, SSE3 en la CPU
            (Estando solo SSE, y SS2, la cpu no soporta SSE3):
                a.has_any_feature("SSE", "SSE2", "SSE3") == True
                # a pesar de que SSE3 no se soporte, se devolvera True pues se dio al menos una
        Returns:
            _type_: _description_
        """
        return any(self.has_feature(f) for f in features)

            
    def print_vals(self) -> None:
        """
            Imprime todas las tecnologias que soportan la CPU, ejemplo:
            >>> a.print_vals()
            Detalles de la CPU:
            ==============================
            FPU       : Soportado
            VME       : Soportado
            DE        : Soportado
            PSE       : Soportado
            TSC       : Soportado
            MSR       : Soportado
            PAE       : Soportado
            MCE       : Soportado
            CX8       : Soportado
            APIC      : Soportado
            RESERVADO : No soportado
            SEP       : Soportado
            MTRR      : Soportado
            PGE       : Soportado
            MCA       : Soportado
            CMOV      : Soportado
            PAT       : Soportado
            PSE36     : Soportado
            PSN       : No soportado
            CLFLUSH   : Soportado
            NX        : No soportado
            DS        : Soportado
            ACPI      : Soportado
            MMX       : Soportado
            FXSR      : Soportado
            SSE       : Soportado
            SSE2      : Soportado
            SS        : Soportado
            HTT       : Soportado
            TM        : Soportado
            IA64      : No soportado
            PBE       : Soportado
            SSE3      : Soportado
            PCLMUL    : Soportado
            DTES64    : Soportado
            MONITOR   : Soportado
            DS_CPL    : Soportado
            VMX       : Soportado
            SMX       : No soportado
            EST       : Soportado
            TM2       : Soportado
            SSSE3     : Soportado
            CID       : No soportado
            SDBG      : Soportado
            FMA       : Soportado
            CX16      : Soportado
            XTPR      : Soportado
            PDCM      : Soportado
            RESERVADO : No soportado
            PCID      : Soportado
            DCA       : No soportado
            SSE4_1    : Soportado
            SSE4_2    : Soportado
            X2APIC    : Soportado
            MOVBE     : Soportado
            POPCNT    : Soportado
            TSC       : Soportado
            AES       : Soportado
            XSAVE     : Soportado
            OSXSAVE   : Soportado
            AVX       : Soportado
            F16C      : Soportado
            RDRAND    : Soportado
            HYPERVISOR: No soportado    
        """
        print("Detalles de la CPU:")
        print("=" * 30)
        for feature_list in self._features:
            for feature, _ in feature_list:
                value = getattr(self, feature, False)
                print(f"{feature:<10}: {'Soportado' if value else 'No soportado'}")


class Additional_Information_Feature_Bits:
    """
    >>> a = cpu_id_x86.Processor_Info_and_Feature_Bits()
    >>> print(a.additional_info)
    Brand Index: 0
    CLFLUSH Line Size: 64 bytes
    Max Addressable IDs: 128
    Local APIC ID: 24
    >>> a.additional_info.CLFLUSH_Line_Size
    64
    
    typedef struct Additional_Information_Feature_Bits { // para CPUID con EAX=1; esta estructura esta en EBX
        uint32_t Brand_Index        :8; /* 7  - 0   Descripción: El Brand Index es un identificador único del modelo del procesador.
                                         *          Validez: Siempre válido.
                                         *          Uso: Se utiliza para identificar y diferenciar entre modelos de procesadores de una misma familia.
                                         */
        uint32_t CLFLUSH            :8; /* 15 - 8   tamaño de línea (Valor * 8 = tamaño de línea de caché en bytes)
                                         *          Descripción: Tamaño de la línea de caché para la instrucción CLFLUSH.
                                         *          Validez: Este campo es válido si el bit 19 de CPUID.01.EDX (CLFSH) está establecido en 1.
                                         *          Cálculo: El valor en estos bits multiplicado por 8 da el tamaño de la línea de caché en bytes.
                                         */
        uint32_t Max_ID_addressable :8; /* 23 - 16  Número máximo de ID direccionables para procesadores lógicos en este paquete físico;
                                         *          El entero de potencia de 2 más cercano que no sea menor que este valor es el número 
                                         *          de ID de APIC iniciales únicos reservados para direccionar diferentes procesadores lógicos en un paquete físico.
                                         *          Uso anterior: Número de procesadores lógicos por procesador físico; dos para el 
                                         *          procesador Pentium 4 con tecnología Hyper-Threading.
                                         *          En CPU con más de 128 procesadores lógicos en un solo paquete (por ejemplo, 
                                         *          Intel Xeon Phi 7290[41] y AMD Threadripper Pro 7995WX[42]), el valor en el bit 23:16 se 
                                         *          establece en un valor sin potencia de 2.
                                         *          
                                         *          Descripcion:
                                         *          Concepto Básico;
                                         *          Los procesadores modernos pueden ejecutar múltiples hilos de ejecución al mismo tiempo. 
                                         *          Esto es posible gracias a tecnologías como el Hyper-Threading de Intel, donde un solo procesador 
                                         *          físico puede actuar como si fuera varios procesadores lógicos. Cada uno de estos procesadores lógicos 
                                         *          puede ejecutar su propio hilo de ejecución.
                                         *          Número Máximo de IDs Direccionables para Procesadores Lógicos
                                         *          Imagina que tienes una fábrica con varias líneas de producción. Cada línea de producción puede trabajar en 
                                         *          una tarea diferente al mismo tiempo. En nuestro ejemplo, cada línea de producción es como un procesador lógico
                                         *          dentro de un procesador físico.
                                         *          El "Número máximo de IDs direccionables para procesadores lógicos" es como el número máximo de líneas de 
                                         *          producción que puede tener tu fábrica. Este número indica cuántos procesadores lógicos diferentes puede 
                                         *          gestionar tu procesador físico.
                                         *          El "Número máximo de IDs direccionables para procesadores lógicos" es como el número máximo de líneas de 
                                         *          producción que puede tener tu fábrica. Este número indica cuántos procesadores lógicos diferentes puede 
                                         *          gestionar tu procesador físico.
                                         *          Por Qué es Importante?
                                         *          Identificación Única:
                                         *          Cada procesador lógico necesita una identificación única para que el sistema operativo y las aplicaciones 
                                         *          puedan comunicarse con él de manera efectiva. Es como tener un número de identificación único para cada línea de producción.
                                         *          Optimización del Rendimiento:
                                         *          Sabiendo cuántos procesadores lógicos puede manejar el procesador físico, el sistema operativo puede optimizar cómo 
                                         *          distribuir las tareas entre ellos. Es similar a asignar diferentes tareas a cada línea de producción para aprovechar 
                                         *          al máximo la capacidad de la fábrica.
                                         *          Configuración del Sistema:
                                         *          Esta información ayuda a configurar correctamente el sistema, especialmente en entornos de servidores o estaciones de 
                                         *          trabajo donde es crucial aprovechar al máximo los recursos del hardware.
                                         *          Ejemplo Simplificado
                                         *          Supongamos que tienes un procesador físico que, gracias a Hyper-Threading, puede manejar hasta 8 procesadores lógicos. 
                                         *          Esto significa que el sistema operativo puede ver y utilizar hasta 8 procesadores como si fueran independientes, aunque 
                                         *          físicamente solo haya uno.
                                         *          Procesador Físico: 1
                                         *          Procesadores Lógicos: 8
                                         *          Número máximo de IDs direccionables: 8
                                         *          Este número máximo (8 en nuestro ejemplo) es crucial para entender cuántos hilos de ejecución puedes correr 
                                         *          simultáneamente en tu sistema.
                                         *          En Resumen
                                         *          El "Número máximo de IDs direccionables para procesadores lógicos" es un valor que indica cuántos procesadores 
                                         *          lógicos puede manejar un procesador físico. Esto permite que el sistema operativo y las aplicaciones optimicen el uso del 
                                         *          hardware, asegurando que las tareas se distribuyan eficientemente entre los distintos procesadores lógicos disponibles
                                         */


        uint32_t Local_APIC_ID      :8; /*  
                                         * El ID de APIC local también se puede identificar a través de la hoja cpuid 0Bh 
                                         *  ( CPUID.0Bh.EDX[x2APIC-ID] ). En CPU con más de 256 procesadores lógicos en un paquete 
                                         *  (por ejemplo, Xeon Phi 7290), se debe utilizar la hoja 0Bh porque el ID APIC no cabe en 8 bits.
                                         *  Descripción: ID APIC local del procesador lógico que está ejecutando la instrucción. APIC 
                                         *  (Advanced Programmable Interrupt Controller) es un componente que maneja interrupciones en sistemas multiprocesador
                                         *  El "Local APIC ID" es importante porque permite identificar de forma única cada procesador lógico. En sistemas con 
                                         *  múltiples procesadores, puede haber conflictos o problemas si dos procesadores tienen el mismo identificador de APIC local.
                                         *  El "Local APIC ID" se utiliza en el manejo de interrupciones y en la coordinación de tareas entre los diferentes 
                                         *  procesadores en un sistema multiprocesador. Cada procesador utiliza su identificador de APIC local para comunicarse 
                                         *  con el APIC local y coordinar las interrupciones y otras actividades del sistema.
                                         *  Validez: Válido para procesadores Pentium 4 y posteriores.
                                         * Uso: El APIC-ID inicial se utiliza para identificar el procesador lógico en ejecución.
                                         * resumen: es un identificador único asignado a cada procesador lógico en un sistema multiprocesador 
                                         * y se utiliza para coordinar las actividades del sistema
                                         */
                                    
    } Additional_Information_Feature_Bits;
    """
    def __init__(self, ebx:Optional[int] = None):
        
        if ebx == None:
            # para amd a de llamarse a 0x80000008
            my_request_cpuid = Cpuid(Register(
                CPUID_GETFEATURES,
                0, 0, 0
            ))
            ebx = my_request_cpuid.reg.ebx
            
        self.Brand_Index         = ebx & 0xFF
        self.CLFLUSH_Line_Size   = ((ebx >> 8) & 0xFF) * 8
        self.Max_Addressable_IDs = (ebx >> 16) & 0xFF # cantidad de procasadores logicos
        self.Local_APIC_ID       = (ebx >> 24) & 0xFF

    def __str__(self):
        return (f"Brand Index: {self.Brand_Index}\n"
                f"CLFLUSH Line Size: {self.CLFLUSH_Line_Size} bytes\n"
                f"Max Addressable IDs: {self.Max_Addressable_IDs}\n"
                f"Local APIC ID: {self.Local_APIC_ID}")
