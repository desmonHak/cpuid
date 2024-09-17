#include "Class.h"
#ifndef __CLASS_CPUID_C__
#define __CLASS_CPUID_C__
#include "Cpuid.h"

static void Method_Cpuid_Class(init_type_object)(void) {
    Method_Cpuid_Class(type_Class) = (PyTypeObject){
        /*
        * Este tipo también tiene algunos métodos personalizados, por lo que necesitamos pasar 
        * una matriz a PyTypeObject.tp_methods.
        * Finalmente, necesitamos construir nuestros nuevos objetos para tener una función 
        * PyTypeObject.tp_new configurada.
        * Esta es la definición de una clase llamada Cpuid de Python y una Cpuid de C. Las 
        * instancias se definen mediante la estructura de cola en la parte superior. El tipo 
        * implementa solo un subconjunto de las funciones posibles.
        * Preparar un tipo
        * Nuestra estructura Cpuid_type tiene muchos miembros NULL porque queremos heredar 
        * su valor de nuestra clase base, que en este caso es object. Si volvemos a mirar 
        * el código en PyObject_Repr() anterior, podemos ver que no estamos haciendo una 
        * comprobación de NULL ni un recorrido de clase, simplemente obtenemos 
        * PyTypeObject.tp_repr y lo llamamos. Para evitar estas comprobaciones, Python 
        * tiene una función llamada PyType_Ready() que copia los slots de la clase base. 
        * Esta función también copia todos los métodos que están almacenados en PyTypeObject.tp_dict.
        * 
        * Para preparar un tipo, llame a PyType_Ready() en PyMODINIT_FUNC para el módulo 
        * que define la clase.
        */
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "cpuid.Cpuid",                                               /* tp_name */
        sizeof(Cpuid),                                               /* tp_basicsize 
                                                                     * PyTypeObject.tp_basicsize se establece para que 
                                                                     * tenga el tamaño de la estructura de cola de salida. 
                                                                     * Esto significa que las instancias de una Cpuid tienen 
                                                                     * el tamaño de una Cpuid.
                                                                     */
        0,                                                           /* tp_itemsize 
                                                                     * PyTypeObject.tp_itemsize se establece en 0 porque todas las 
                                                                     * instancias de una Cpuid usan la misma cantidad de espacio. 
                                                                     * Este campo está diseñado para implementar colecciones 
                                                                     * compactas como tuple.
                                                                     */
        (destructor)Method_Cpuid_Class(dealloc),                 /* tp_dealloc 
                                                                     * PyTypeObject.tp_dealloc se establece en una función 
                                                                     * de desasignación. Esta función necesita liberar 
                                                                     * la referencia de la cola a q_elements.
                                                                     */
        0,                                                           /* tp_print */
        0,                                                           /* tp_getattr */
        0,                                                           /* tp_setattr */
        0,                                                           /* tp_reserved */
        (reprfunc)Method_Cpuid_Class(repr),                       /* tp_repr */
        0,                                                           /* tp_as_number */
        0,                                                           /* tp_as_sequence */
        0,                                                           /* tp_as_mapping */
        0,                                                           /* tp_hash */
        0,                                                           /* tp_call */
        0,                                                           /* tp_str */
        0,                                                           /* tp_getattro */
        0,                                                           /* tp_setattro */
        0,                                                           /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_HAVE_GC,                                          /* tp_flags 
                                                                      * PyTypeObject.tp_flags se establece en 
                                                                      * Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC. Esto 
                                                                      * significa que tenemos tanto los indicadores predeterminados 
                                                                      * como el bit HAVE_GC. Debido a que el bit 
                                                                      * Py_TPFLAGS_HAVE_GC está configurado, también tenemos una 
                                                                      * función PyTypeObject.tp_traverse y una función 
                                                                      * PyTypeObject.tp_clear.
                                                                      */
        .tp_doc             = Method_Cpuid_Class(doc),                             /* tp_doc */ // != PyDoc_STR
        .tp_traverse        = (traverseproc) Method_Cpuid_Class(traverse),         /* tp_traverse */ //(traverseproc) Cpuid_traverse,
        .tp_clear           = (inquiry) Method_Cpuid_Class(clear),                 /* tp_clear */ //(inquiry) Cpuid_clear,        
        .tp_richcompare     = 0,                                     /* tp_richcompare */
        .tp_weaklistoffset  = 0,                                     /* tp_weaklistoffset */
        .tp_iter            = 0,                                     /* tp_iter */
        .tp_iternext        = 0,                                     /* tp_iternext */
        .tp_methods         = Method_Cpuid_Class(methods),                         /* tp_methods */
        .tp_members         = 0,                                     /* tp_members */
        .tp_getset          = 0,                                     /* tp_getset 
                                                                      * Metodos getter y setter para los atributos de 
                                                                      * la clase.
                                                                      */
        .tp_base            = 0,                                     /* tp_base */
        .tp_dict            = 0,                                     /* tp_dict */
        .tp_descr_get       = 0,                                     /* tp_descr_get */
        .tp_descr_set       = 0,                                     /* tp_descr_set */
        .tp_dictoffset      = 0,                                     /* tp_dictoffset */
        .tp_init            = (initproc)Method_Cpuid_Class(init), /* 
                                                                      * tp_init 
                                                                      * MyClass.__init__ --> tp_init
                                                                      */
        .tp_alloc           = 0,                                      /* tp_alloc */
        .tp_new             = Method_Cpuid_Class(new),             /* tp_new 
                                                                       * Si PyTypeObject.tp_new es NULL, no puede crear 
                                                                       * nuevas instancias de la clase.
                                                                       */
    };
    // Finalmente, llamamos a PyType_Ready
    if (PyType_Ready(&Method_Cpuid_Class(type_Class)) < 0)
        return;
}

// Método __init__
static int Method_Cpuid_Class(init)(Cpuid *self, PyObject *args, PyObject *kwds) {
    PyObject *reg_obj = NULL;
    static char *kwlist[] = {"register", NULL};

    Py_ssize_t maxsize = 10;  // valor por defecto
    
    // Parseamos los argumentos de __init__(self, value=0)
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &Register_type_Class, &reg_obj)) {
        PyErr_SetString(PyExc_TypeError, "Se requiere una instancia de Register");
        return -1;
    }
    
    self->reg = (Register *)reg_obj;
    Py_INCREF(self->reg);  // Incrementamos la referencia al objeto Register

    debug_print_cpuid("Parsed args: maxsize=%zd,eax=0x%08x, ebx=0x%08x, ecx=0x%08x, edx=0x%08x\n",
        maxsize, self->reg->eax, self->reg->ebx, self->reg->ecx, self->reg->edx);

    call_cpuid(
        self->reg->eax, self->reg->ebx, 
        &(self->reg->eax), &(self->reg->ebx),
        &(self->reg->ecx), &(self->reg->edx)
    );
    debug_print_cpuid("Before Call cpuid: maxsize=%zd,eax=0x%08x, ebx=0x%08x, ecx=0x%08x, edx=0x%08x\n",
        maxsize, self->reg->eax, self->reg->ebx, self->reg->ecx, self->reg->edx);

    // Asegúrate de que q_elements esté inicializado
    if (self->q_elements == NULL) {
        debug_print_cpuid("Creating new q_elements\n");
        self->q_elements = PyList_New(0);
        if (self->q_elements == NULL) {
            debug_print_cpuid("Failed to create q_elements\n");
            return -1;
        }
    }
    debug_print_cpuid("Exiting Cpuid_init successfully\n");
    return 0;
}

static PyObject *Method_Cpuid_Class(new)(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    debug_print_cpuid("Entering Cpuid_new\n");
    Cpuid *self;
    self = (Cpuid *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->value = 0;
        self->q_maxsize = 10;  // valor por defecto
        self->q_elements = PyList_New(0);
        self->reg = NULL;  // Inicializamos el puntero a NULL
        if (self->q_elements == NULL) {
            debug_print_cpuid("Failed to create q_elements in new\n");
            Py_DECREF(self);
            return NULL;
        }
    } else {
        debug_print_cpuid("Failed to allocate Cpuid\n");
        return NULL;
    }
    debug_print_cpuid("Exiting Cpuid_new successfully\n");
    return (PyObject *)self;
}

// Método cpuid
static PyObject* Method_Cpuid_Class(cpuid)(Cpuid *self, PyObject *args) {
    CPUID_H(registers) my_regs = {
        .eax = 0, .ebx = 0,
        .ecx = 0, .edx = 0
    };
    
    // Parseamos los argumentos de cpuid(self, eax, ecx)
    if (!PyArg_ParseTuple(args, "kk", &(my_regs.eax), &(my_regs.ecx))) {
        PyErr_SetString(PyExc_TypeError, "Se esperan dos argumentos: eax(pagina) y ecx(subpagina)");
        return NULL;
    }
    debug_print_cpuid("Tey call Cpuid(EAX = 0x%08x, ECX = 0x%08x)\n");
    
    //is_cpuid_supported()
    call_cpuid(
        my_regs.eax, my_regs.ecx, 
        &(my_regs.eax), &(my_regs.ebx),
        &(my_regs.ecx), &(my_regs.edx)
    );
    self->reg->eax = my_regs.eax; self->reg->ebx = my_regs.ebx;
    self->reg->ecx = my_regs.ecx; self->reg->edx = my_regs.edx;
    PyObject *reg_args = Py_BuildValue(
        "kkkk", my_regs.eax, my_regs.ebx, my_regs.ecx, my_regs.edx
    );
    PyObject *reg_obj = PyObject_CallObject(
        (PyObject *) &Method_Register_Class(type_Class), 
        reg_args
    );
    Py_DECREF(reg_args);
    
    if (reg_obj == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "No se pudo crear una instancia de Register");
        return NULL;
    }


    // Retornamos un objeto de tipo registro
    return reg_obj;
}

static void Method_Cpuid_Class(dealloc)(Cpuid *self) {
    // Desvinculamos el objeto del recolector de basura (si es necesario)
    PyObject_GC_UnTrack(self);

    // Liberamos la referencia al objeto q_elements (si es necesario)
    Py_CLEAR(self->q_elements);
    /*
     * Py_CLEAR es una macro de seguridad que se asegura de reducir la 
     * referencia del campo q_elements a NULL correctamente. Esto es 
     * importante para evitar liberar recursos que podrían estar siendo 
     * utilizados en otro lugar.
     */

    Py_XDECREF(self->reg);  // Liberamos la referencia a Register
    
    // Llamamos al deallocador del tipo para liberar la memoria de self
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* Method_Cpuid_Class(repr)(Cpuid *self) {
    PyObject *result = NULL;
    debug_print_cpuid("Entering Cpuid_repr\n");
    
    if (self == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "self is NULL in Cpuid_repr");
        return NULL;
    }
    
    Py_ssize_t element_count = self->q_elements ? PyList_Size(self->q_elements) : 0;
    /*if (self->q_elements != NULL) {
        if (PyList_Check(self->q_elements)) {
            element_count = PyList_Size(self->q_elements);
        } else {
            PyErr_SetString(PyExc_RuntimeError, "q_elements is not a list");
            return NULL;
        }
    } else {
        debug_print_cpuid("Warning: q_elements is NULL in repr\n");
    }*/

    debug_print_cpuid("Creating repr string\n");
    result = PyUnicode_FromFormat(
        "Cpuid(maxsize=%zd, current_elements=%zd, eax=0x%08x, ebx=0x%08x, ecx=0x%08x, edx=0x%08x)",
        self->q_maxsize, element_count, self->reg->eax, self->reg->ebx, 
        self->reg->ecx, self->reg->edx
    );
    
    if (result == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create repr string");
        return NULL;
    }
    
    debug_print_cpuid("Exiting Cpuid_repr successfully\n");
    return result;
}
static int Method_Cpuid_Class(traverse)(Cpuid *self, visitproc visit, void *arg) {
    // Si q_elements tiene una referencia a un objeto Python, la recorremos
    Py_VISIT(self->q_elements);
    Py_VISIT(self->reg);
    return 0;
}
static int Method_Cpuid_Class(clear)(Cpuid *self) {
    // Eliminamos la referencia a q_elements
    Py_CLEAR(self->q_elements);
    Py_CLEAR(self->reg);
    return 0;
}

#endif
