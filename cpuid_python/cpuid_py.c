#include <Python.h>
/*
Incluido <stdio.h>, <string.h>, <errno.h>, y <stdlib.h> 
*/

// Modo debug activado:
#define DEBUG_ENABLE_CPUID

#include "global.h"

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
    PyModule_AddIntConstant(module, "FPUTS_FLAG", 64);

    /* Define int macro */
    #define FPUTS_MACRO 256

    /* Add macro to module */
    PyModule_AddIntMacro(module, FPUTS_MACRO);

    return module; // retornar el modulo creado.
}