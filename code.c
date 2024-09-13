#include "cpuid.h"
#include <windows.h>

int main() {
    printProcessor_Info_and_Feature_Bits(0x756e6547);
    printInformation_Feature_Bits(0x49656e69, 0x6c65746e);
    return 0;
}