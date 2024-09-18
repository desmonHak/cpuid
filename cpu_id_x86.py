from cpuid_x86 import Cpuid, Register, CPUID_GETFEATURES

""" 
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
class Processor_Info_and_Feature_Bits():
    def __init__(self, eax:int = None):
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
                                       (self.Model << 4) + \
                                       (self.Stepping_ID)
        
    def get_model_number(self, Family_ID:int = None, Model:int = None, Extended_Model_ID:int = None):
        #((Family_ID == 0x06) || (Family_ID == 0x0f)) ? ((Extended_Model_ID << 4) + Model_ID) : Model_ID
        if (Family_ID == None): Family_ID = self.Family_ID
        if (Model == None): Model = self.Model
        if (Extended_Model_ID == None): Extended_Model_ID = self.Extended_Model_ID
        
        return (
            (Extended_Model_ID << 4) + Model) \
                if (Family_ID == 0x06 or Family_ID == 0x0f) \
                else Model

    
    def print_vals(self):
        print(
            f"Reserved1           :4  = {hex(self.Reserved1)}\n" +
            f"Extended_Family_ID  :8  = {hex(self.Extended_Family_ID)}\n" +
            f"Extended_Model_ID   :4  = {hex(self.Extended_Model_ID)}\n" +
            f"Reserved2           :2  = {hex(self.Reserved2)}\n" +
            f"Processor_Type      :2  = {hex(self.Processor_Type)}\n" +
            f"Family_ID           :4  = {hex(self.Family_ID)}\n" +
            f"Model               :4  = {hex(self.Model)}\n" +
            f"Stepping_ID         :4  = {hex(self.Stepping_ID)}\n" +
            f"All_Model               = {hex(self.All_Model)}\n"
            f"CPUID signature         = {hex(self.CPUID_signature)}\n"
        )