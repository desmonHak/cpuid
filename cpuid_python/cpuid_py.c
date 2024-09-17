#include <Python.h>
/*
Incluido <stdio.h>, <string.h>, <errno.h>, y <stdlib.h> 
*/

// Modo debug activado:
//#define DEBUG_ENABLE_CPUID

#include "global.h"

// es necesario definir cpuid.c sin incluirlo, pues este se enlazara desde un
// .o, de esta manera compilar desde Mingw32, TDM o cualquier otro que admita
// las macros asm definidas en el archivo cpuid.c
#ifndef __CPUID_C__
#define __CPUID_C__
#else
#error "cpuid.c ya se encuentra incluido"
#endif
#include "../cpuid.h"

// Clases del modulo
#include "Class/Cpuid.h"
#include "Class/Register.h"

FILE *fp;
static PyObject *StringTooShortError = NULL;

static PyObject *method_fputs(PyObject *self, PyObject *args) {
    /*
     * La función C siempre tiene dos argumentos, llamados convencionalmente self y args.
     * El argumento self apunta al objeto del módulo para funciones a nivel de módulo; para un método 
     * apuntaría a la instancia del objeto. El argumento args será un puntero a un objeto de 
     * tupla de Python que contiene los argumentos.
     */
    char *str, *filename = NULL;
    int bytes_copied = -1;

    /* Parse arguments 
     * La función PyArg_ParseTuple() en la API de Python verifica los tipos de argumento y los convierte a valores C.
     */
    // https://www.mit.edu/people/amliu/vrut/python/ext/parseTuple.html
    if(!PyArg_ParseTuple(args, "ss", &str, &filename)) {
        return NULL;
    }

    debug_print_cpuid("Archivo %s, con texto %s creado con exito\n", filename, str);

    FILE *fp = fopen(filename, "w");
    bytes_copied = fputs(str, fp);
    fclose(fp);

    return PyLong_FromLong(bytes_copied);
}

/*
 * No usamos simplemente un const char* normal para la cadena de documentación 
 * porque CPython se puede compilar para que no incluya cadenas de documentación. 
 * Esto es útil en plataformas con menos RAM disponible. Para respetar adecuadamente 
 * esta opción de tiempo de compilación, envolvemos todas las cadenas de documentación 
 * en la macro PyDoc_STRVAR().
 */
PyDoc_STRVAR(cpuid_doc, "Func CPUID");
PyDoc_STRVAR(cpuid_module_doc, "Modulo CPUID");

/*
 * Funciones del modulo con sus metadatos
 */
static PyMethodDef cpuid_native_methods[] = {
    {
        "cpuid", 
        (PyCFunction)method_fputs, 
        METH_VARARGS, 
        cpuid_doc
    },
    {NULL, NULL, 0, NULL}
};

/*
 *
 * Metadatos del modulo
 * 
 */
static struct PyModuleDef cpuid_native = {
    PyModuleDef_HEAD_INIT,
    "cpuid",
    cpuid_module_doc,
    -1,
    cpuid_native_methods,
    NULL,
    NULL,
    NULL,
    NULL
};

/*
 *
 * Funcion inicializadora del modulo:
 * Una vez que definimos nuestra función y nuestro módulo, debemos indicarle a CPython cómo importar nuestro módulo. 
 * Para ello, debemos definir una única función con el tipo PyMODINIT_FUNC denominada PyInit_{name}, donde name es el 
 * nombre de nuestro módulo.
 * Esta función se ejecutará la primera vez que alguien escriba import fib.fib from python. Esto 
 * se puede considerar como el código que se ejecuta en el "ámbito del módulo" en un archivo Python normal. 
 * Al final de nuestra función, debemos devolver el módulo recién creado. Para crear un PyObject* a 
 * partir de un PyModule_Def, podemos usar PyModule_Create().
 * 
 */
PyMODINIT_FUNC PyInit_cpuid_x86(void) {
    _ACTIVATE_COLORS_ANSI_WIN__();

    // Clase Cpuid
    Method_Cpuid_Class(init_type_object)();
    if (PyType_Ready(&Method_Cpuid_Class(type_Class)) < 0) {
        debug_print_cpuid("PyType_Ready Cpuid Class failed\n");
        return NULL;
    }
    
    // Clase Registro
    Method_Register_Class(init_type_object)();
    if (PyType_Ready(&Method_Register_Class(type_Class)) < 0) {
        debug_print_cpuid("PyType_Ready Register Class failed\n");
        return NULL;
    }


    /* Assign module value */
    PyObject *module = PyModule_Create(&cpuid_native);
    if (module == NULL) return NULL;

    // Clase Cpuid
    Py_INCREF(&Method_Cpuid_Class(type_Class));
    if (PyModule_AddObject(module, "Cpuid", (PyObject *) &Method_Cpuid_Class(type_Class)) < 0) {
        Py_DECREF(&Method_Cpuid_Class(type_Class));
        Py_DECREF(module);
        return NULL;
    }
    // Clase registro
    Py_INCREF(&Method_Register_Class(type_Class));
    if (PyModule_AddObject(module, "Register", (PyObject *) &Method_Register_Class(type_Class)) < 0) {
        Py_DECREF(&Method_Register_Class(type_Class));
        Py_DECREF(module);
        return NULL;
    }


    /* Add int constant by name */
    //PyModule_AddIntConstant(module, "FPUTS_FLAG", 64);

    // CPUID_FEAT
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SSE3", CPUID_FEAT_ECX_SSE3);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_PCLMUL", CPUID_FEAT_ECX_PCLMUL);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_DTES64", CPUID_FEAT_ECX_DTES64);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_MONITOR", CPUID_FEAT_ECX_MONITOR);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_DS_CPL", CPUID_FEAT_ECX_DS_CPL);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_VMX", CPUID_FEAT_ECX_VMX);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SMX", CPUID_FEAT_ECX_SMX);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_EST", CPUID_FEAT_ECX_EST);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_TM2", CPUID_FEAT_ECX_TM2);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SSSE3", CPUID_FEAT_ECX_SSSE3);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_CID", CPUID_FEAT_ECX_CID);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SDBG", CPUID_FEAT_ECX_SDBG);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_FMA", CPUID_FEAT_ECX_FMA);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_CX16", CPUID_FEAT_ECX_CX16);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_XTPR", CPUID_FEAT_ECX_XTPR);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_PDCM", CPUID_FEAT_ECX_PDCM);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_RESERVADO", CPUID_FEAT_ECX_RESERVADO);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_PCID", CPUID_FEAT_ECX_PCID);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_DCA", CPUID_FEAT_ECX_DCA);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SSE4_1", CPUID_FEAT_ECX_SSE4_1);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_SSE4_2", CPUID_FEAT_ECX_SSE4_2);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_X2APIC", CPUID_FEAT_ECX_X2APIC);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_MOVBE", CPUID_FEAT_ECX_MOVBE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_POPCNT", CPUID_FEAT_ECX_POPCNT);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_TSC", CPUID_FEAT_ECX_TSC);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_AES", CPUID_FEAT_ECX_AES);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_XSAVE", CPUID_FEAT_ECX_XSAVE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_OSXSAVE", CPUID_FEAT_ECX_OSXSAVE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_AVX", CPUID_FEAT_ECX_AVX);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_F16C", CPUID_FEAT_ECX_F16C);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_RDRAND", CPUID_FEAT_ECX_RDRAND);
    PyModule_AddIntConstant(module, "CPUID_FEAT_ECX_HYPERVISOR", CPUID_FEAT_ECX_HYPERVISOR);

    // para CPUID EAX=1: Feature Information in EDX
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_FPU", CPUID_FEAT_EDX_FPU);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_VME", CPUID_FEAT_EDX_VME);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_DE", CPUID_FEAT_EDX_DE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PSE", CPUID_FEAT_EDX_PSE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_TSC", CPUID_FEAT_EDX_TSC);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_MSR", CPUID_FEAT_EDX_MSR);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PAE", CPUID_FEAT_EDX_PAE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_MCE", CPUID_FEAT_EDX_MCE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_CX8", CPUID_FEAT_EDX_CX8);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_APIC", CPUID_FEAT_EDX_APIC);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_RESERVADO", CPUID_FEAT_EDX_RESERVADO);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_SEP", CPUID_FEAT_EDX_SEP);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_MTRR", CPUID_FEAT_EDX_MTRR);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PGE", CPUID_FEAT_EDX_PGE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_MCA", CPUID_FEAT_EDX_MCA);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_CMOV", CPUID_FEAT_EDX_CMOV);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PAT", CPUID_FEAT_EDX_PAT);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PSE36", CPUID_FEAT_EDX_PSE36);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PSN", CPUID_FEAT_EDX_PSN);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_CLFLUSH", CPUID_FEAT_EDX_CLFLUSH);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_NX", CPUID_FEAT_EDX_NX);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_DS", CPUID_FEAT_EDX_DS);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_ACPI", CPUID_FEAT_EDX_ACPI);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_MMX", CPUID_FEAT_EDX_MMX);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_FXSR", CPUID_FEAT_EDX_FXSR);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_SSE", CPUID_FEAT_EDX_SSE);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_SSE2", CPUID_FEAT_EDX_SSE2);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_SS", CPUID_FEAT_EDX_SS);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_HTT", CPUID_FEAT_EDX_HTT);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_TM", CPUID_FEAT_EDX_TM);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_IA64", CPUID_FEAT_EDX_IA64);
    PyModule_AddIntConstant(module, "CPUID_FEAT_EDX_PBE", CPUID_FEAT_EDX_PBE);

    /* Define int macro */
    //#define FPUTS_MACRO 256

    /* Add macro to module */
    PyModule_AddIntMacro(module, CPUID_VENDOR_AMD);
    PyModule_AddIntMacro(module, CPUID_VENDOR_AMD_OLD);
    PyModule_AddIntMacro(module, CPUID_VENDOR_INTEL);
    PyModule_AddIntMacro(module, CPUID_VENDOR_VIA);
    PyModule_AddIntMacro(module, CPUID_VENDOR_TRANSMETA);
    PyModule_AddIntMacro(module, CPUID_VENDOR_TRANSMETA_OLD);
    PyModule_AddIntMacro(module, CPUID_VENDOR_CYRIX);
    PyModule_AddIntMacro(module, CPUID_VENDOR_CENTAUR);
    PyModule_AddIntMacro(module, CPUID_VENDOR_NEXGEN);
    PyModule_AddIntMacro(module, CPUID_VENDOR_UMC);
    PyModule_AddIntMacro(module, CPUID_VENDOR_SIS);
    PyModule_AddIntMacro(module, CPUID_VENDOR_NSC);
    PyModule_AddIntMacro(module, CPUID_VENDOR_RISE);
    PyModule_AddIntMacro(module, CPUID_VENDOR_VORTEX);
    PyModule_AddIntMacro(module, CPUID_VENDOR_AO486);
    PyModule_AddIntMacro(module, CPUID_VENDOR_AO486_OLD);
    PyModule_AddIntMacro(module, CPUID_VENDOR_ZHAOXIN);
    PyModule_AddIntMacro(module, CPUID_VENDOR_HYGON);
    PyModule_AddIntMacro(module, CPUID_VENDOR_ELBRUS);
    
    PyModule_AddIntMacro(module, CPUID_VENDOR_HYPERV);
    PyModule_AddIntMacro(module, CPUID_VENDOR_KVM1);
    PyModule_AddIntMacro(module, CPUID_VENDOR_KVM2);
    PyModule_AddIntMacro(module, CPUID_VENDOR_KVM3);
    PyModule_AddIntMacro(module, CPUID_VENDOR_BHYVE1);
    PyModule_AddIntMacro(module, CPUID_VENDOR_BHYVE2);
    PyModule_AddIntMacro(module, CPUID_VENDOR_XEN);
    PyModule_AddIntMacro(module, CPUID_VENDOR_QEMU);
    PyModule_AddIntMacro(module, CPUID_VENDOR_PARALLELS_ALT);
    PyModule_AddIntMacro(module, CPUID_VENDOR_PARALLELS);
    PyModule_AddIntMacro(module, CPUID_VENDOR_VMWARE);
    PyModule_AddIntMacro(module, CPUID_VENDOR_PPROJECT_ACRN);
    PyModule_AddIntMacro(module, CPUID_VENDOR_VIRTUALBOX);
    PyModule_AddIntMacro(module, CPUID_VENDOR_QNX1);
    PyModule_AddIntMacro(module, CPUID_VENDOR_QNX2);

    PyModule_AddIntMacro(module, CPUID_VENDOR_NETBSD_NVMM);
    PyModule_AddIntMacro(module, CPUID_VENDOR_OPENBSD_VMM);
    PyModule_AddIntMacro(module, CPUID_VENDOR_INTEL_HAXM);
    PyModule_AddIntMacro(module, CPUID_VENDOR_INTEL_KGT);
    PyModule_AddIntMacro(module, CPUID_VENDOR_UNISYS_S_PAR);
    PyModule_AddIntMacro(module, CPUID_VENDOR_LOCKHEED_MARTIN_LMHS);

    // Processor Type values:
    PyModule_AddIntMacro(module, OEM_Processor);
    PyModule_AddIntMacro(module, Intel_Overdrive_Processor);
    PyModule_AddIntMacro(module, Dual_processor);
    PyModule_AddIntMacro(module, Processor_Type_value_reserved);

    PyModule_AddIntMacro(module, CPUID_GETVENDORSTRING);
    PyModule_AddIntMacro(module, CPUID_GETFEATURES);
    PyModule_AddIntMacro(module, CPUID_GETTLB);
    PyModule_AddIntMacro(module, CPUID_GETSERIAL);
    PyModule_AddIntMacro(module, CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_INTEL);
    PyModule_AddIntMacro(module, CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY1);
    PyModule_AddIntMacro(module, CPUID_MONITOR_MWAIT_FEATURES);
    PyModule_AddIntMacro(module, CPUID_THERMAL_AND_POWER_MANAGEMENT);
    PyModule_AddIntMacro(module, CPUID_Extended_Features);
    PyModule_AddIntMacro(module, CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY2);
    PyModule_AddIntMacro(module, CPUID_XSAVE_FEATURES_AND_STATE_COMPONENTS);
    PyModule_AddIntMacro(module, CPUID_SGX_CAPATIBLES);
    PyModule_AddIntMacro(module, CPUID_PROCESSOR_TRACE);
    PyModule_AddIntMacro(module, CPUID_TSC_AND_CORE_CRYSTAL_FREQUENCY_INFORMATION);
    PyModule_AddIntMacro(module, CPUID_PROCESSOR_AND_BUS_SPECIFICATION_FREQUENCIES_INFORMATION);
    PyModule_AddIntMacro(module, CPUID_SOC_VENDOR_ATTRIBUTE_ENUMRATION);
    PyModule_AddIntMacro(module, CPUID_INTEL_KEY_LOCKER_FEATURES);
    PyModule_AddIntMacro(module, CPUID_TYPE_CORE);
    PyModule_AddIntMacro(module, CPUID_RESERVED_FOR_TDX_ENUMERATION);
    PyModule_AddIntMacro(module, CPUID_AVX10_FEATURES);
    PyModule_AddIntMacro(module, CPUID_RESERVED_FOR_HYPERVISOR_USE);
    PyModule_AddIntMacro(module, CPUID_INTELEXTENDED);
    PyModule_AddIntMacro(module, CPUID_INTELFEATURES);
    PyModule_AddIntMacro(module, CPUID_INTELBRANDSTRING);
    PyModule_AddIntMacro(module, CPUID_INTELBRANDSTRINGMORE);
    PyModule_AddIntMacro(module, CPUID_INTELBRANDSTRINGEND);
    PyModule_AddIntMacro(module, CPUID_INTEL_SIZE_CACHE_L1);
    PyModule_AddIntMacro(module, CPUID_INTEL_SIZE_CACHE_L2_L3);
    PyModule_AddIntMacro(module, CPUID_CACHE_HIERARCHY_AND_TOPOLOGY_AMD);

    return module; // retornar el modulo creado.
}