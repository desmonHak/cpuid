#include "Class.h"
#ifndef __CLASS_REGISTER_C__
#define __CLASS_REGISTER_C__
#include "Register.h"

static PyObject *Method_Register_Class(get_eax)(Register *self, void *closure) {
    return PyLong_FromUnsignedLong(self->eax);
}

static int Method_Register_Class(set_eax)(Register *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "No se puede eliminar el atributo eax");
        return -1;
    }
    
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "El valor del atributo eax debe ser un número entero");
        return -1;
    }

    self->eax = (uint32_t)PyLong_AsUnsignedLong(value);
    return 0;
}

static PyObject *Method_Register_Class(get_ebx)(Register *self, void *closure) {
    return PyLong_FromUnsignedLong(self->ebx);
}

static int Method_Register_Class(set_ebx)(Register *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "No se puede eliminar el atributo ebx");
        return -1;
    }
    
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "El valor del atributo ebx debe ser un número entero");
        return -1;
    }

    self->ebx = (uint32_t)PyLong_AsUnsignedLong(value);
    return 0;
}

static PyObject *Method_Register_Class(get_ecx)(Register *self, void *closure) {
    return PyLong_FromUnsignedLong(self->ecx);
}

static int Method_Register_Class(set_ecx)(Register *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "No se puede eliminar el atributo ecx");
        return -1;
    }
    
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "El valor del atributo ecx debe ser un número entero");
        return -1;
    }

    self->ecx = (uint32_t)PyLong_AsUnsignedLong(value);
    return 0;
}

static PyObject *Method_Register_Class(get_edx)(Register *self, void *closure) {
    return PyLong_FromUnsignedLong(self->edx);
}

static int Method_Register_Class(set_edx)(Register *self, PyObject *value, void *closure) {
    if (value == NULL) {
        PyErr_SetString(PyExc_TypeError, "No se puede eliminar el atributo edx");
        return -1;
    }
    
    if (!PyLong_Check(value)) {
        PyErr_SetString(PyExc_TypeError, "The edx attribute value must be an integer");
        return -1;
    }

    self->edx = (uint32_t)PyLong_AsUnsignedLong(value);
    return 0;
}


static void Method_Register_Class(init_type_object)(void) {
    Method_Register_Class(type_Class) = (PyTypeObject){
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "cpuid_x86.Register",                      /* tp_name */
        sizeof(Register),                          /* tp_basicsize 
                                                    * PyTypeObject.tp_basicsize se establece para que 
                                                    * tenga el tamaño de la estructura de cola de salida. 
                                                    * Esto significa que las instancias de una Register tienen 
                                                    * el tamaño de una Register.
                                                    */
        0,                                          /* tp_itemsize 
                                                    * PyTypeObject.tp_itemsize se establece en 0 porque todas las 
                                                    * instancias de una Register usan la misma cantidad de espacio. 
                                                    * Este campo está diseñado para implementar colecciones 
                                                    * compactas como tuple.
                                                    */
        (destructor) Method_Register_Class(dealloc),                 /* tp_dealloc 
                                                    * PyTypeObject.tp_dealloc se establece en una función 
                                                    * de desasignación. Esta función necesita liberar 
                                                    * la referencia de la cola a q_elements.
                                                    */
        0,                                          /* tp_print */
        0,                                          /* tp_getattr */
        0,                                          /* tp_setattr */
        0,                                          /* tp_reserved */
        (reprfunc) Method_Register_Class(repr),                      /* tp_repr */
        0,                                          /* tp_as_number */
        0,                                          /* tp_as_sequence */
        0,                                          /* tp_as_mapping */
        0,                                          /* tp_hash */
        0,                                          /* tp_call */
        0,                                          /* tp_str */
        0,                                          /* tp_getattro */
        0,                                          /* tp_setattro */
        0,                                          /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_HAVE_GC |
        Py_TPFLAGS_BASETYPE,                         /* tp_flags 
                                                    * PyTypeObject.tp_flags se establece en 
                                                    * Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC. Esto 
                                                    * significa que tenemos tanto los indicadores predeterminados 
                                                    * como el bit HAVE_GC. Debido a que el bit 
                                                    * Py_TPFLAGS_HAVE_GC está configurado, también tenemos una 
                                                    * función PyTypeObject.tp_traverse y una función 
                                                    * PyTypeObject.tp_clear.
                                                    */
        .tp_doc             = Method_Register_Class(doc),                 /* tp_doc */ // != PyDoc_STR
        .tp_traverse        = (traverseproc) Method_Register_Class(traverse), /* tp_traverse */ //(traverseproc) Register_traverse,
        .tp_clear           = (inquiry) Method_Register_Class(clear),        /* tp_clear */ //(inquiry) Register_clear,        
        .tp_richcompare     = 0,                         /* tp_richcompare */
        .tp_weaklistoffset  = 0,                         /* tp_weaklistoffset */
        .tp_iter            = 0,                         /* tp_iter */
        .tp_iternext        = 0,                         /* tp_iternext */
        .tp_methods         = Method_Register_Class(methods),          /* tp_methods */
        .tp_members         = 0,                         /* tp_members */
        .tp_getset          = Method_Register_Class(getsetters),                         /* tp_getset */
        .tp_base            = 0,                         /* tp_base */
        .tp_dict            = 0,                         /* tp_dict */
        .tp_descr_get       = 0,                         /* tp_descr_get */
        .tp_descr_set       = 0,                         /* tp_descr_set */
        .tp_dictoffset      = 0,                         /* tp_dictoffset */
        .tp_init            = (initproc)Method_Register_Class(init),  /* 
                                                        * tp_init 
                                                        * MyClass.__init__ --> tp_init
                                                        */
        .tp_alloc           = 0,                          /* tp_alloc */
        .tp_new             = Method_Register_Class(new),            /* tp_new 
                                                        * Si PyTypeObject.tp_new es NULL, no puede crear 
                                                        * nuevas instancias de la clase.
                                                        */
    };
    // Finalmente, llamamos a PyType_Ready
    if (PyType_Ready(&Method_Register_Class(type_Class)) < 0)
        return;
}

// Método __init__
static int Method_Register_Class(init)(Register *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"eax", "ebx", "ecx", "edx", NULL};
    // Inicializamos con valores por defecto
    self->eax = 0;
    self->ebx = 0;
    self->ecx = 0;
    self->edx = 0;
    self->q_maxsize = 10;  // valor por defecto
    
    // Parseamos los argumentos de __init__(self, value=0)
    if (!PyArg_ParseTupleAndKeywords(
            args, kwds, "|IIII", kwlist,
            &self->eax, &self->ebx, &self->ecx, &self->edx)
        ){
        debug_print_cpuid("PyArg_ParseTuple failed\n");
        return -1;
    }
    
    debug_print_cpuid("Parsed args: eax=0x%p, ebx=0x%p, edx=0x%p, ecx=0x%p, maxsize=%zd\n",
                        self->eax, self->ebx, self->ecx, self->edx, self->q_maxsize);

    // Asegúrate de que q_elements esté inicializado
    if (self->q_elements == NULL) {
        debug_print_cpuid("Creating new q_elements\n");
        self->q_elements = PyList_New(0);
        if (self->q_elements == NULL) {
            debug_print_cpuid("Failed to create q_elements\n");
            return -1;
        }
    }
    debug_print_cpuid("Exiting Register_init successfully\n");
    return 0;

}

static PyObject *Method_Register_Class(new)(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    debug_print_cpuid("Entering Register_new\n");
    Register *self;
    self = (Register *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->eax = 0; self->ebx = self->eax; self->ecx = self->eax; self->edx = self->eax;
        self->q_maxsize = 10;  // valor por defecto
        self->q_elements = PyList_New(0);
        if (self->q_elements == NULL) {
            debug_print_cpuid("Failed to create q_elements in new\n");
            Py_DECREF(self);
            return NULL;
        }
    } else {
        debug_print_cpuid("Failed to allocate Register\n");
        return NULL;
    }
    debug_print_cpuid("Exiting Register_new successfully\n");
    return (PyObject *)self;
}

// Método 
/*static PyObject* Method_Register_Class(Register)(Register *self, PyObject *args) {
    int num;
    
    // Parseamos los argumentos de add(self, num)
    if (!PyArg_ParseTuple(args, "i", &num)) {
        return NULL;
    }
    
    // Realizamos la suma
    self->value += num;
    
    // Retornamos el nuevo valor
    return PyLong_FromLong(self->value);
}*/

static void Method_Register_Class(dealloc)(Register *self) {
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
    
    // Llamamos al deallocador del tipo para liberar la memoria de self
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject* Method_Register_Class(repr)(Register *self) {
    PyObject *result = NULL;
    debug_print_cpuid("Entering Register_repr\n");
    
    if (self == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "self is NULL in Register_repr");
        return NULL;
    }
    
    Py_ssize_t element_count = 0;
    if (self->q_elements != NULL) {
        if (PyList_Check(self->q_elements)) {
            element_count = PyList_Size(self->q_elements);
        } else {
            PyErr_SetString(PyExc_RuntimeError, "q_elements is not a list");
            return NULL;
        }
    } else {
        debug_print_cpuid("Warning: q_elements is NULL in repr\n");
    }

    debug_print_cpuid("Creating repr string\n");
    result = PyUnicode_FromFormat(
        "Register(maxsize=%zd, current_elements=%zd, "
        "eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x)",
        self->q_maxsize, element_count,
        self->eax, self->ebx, self->ecx, self->edx
    );
    
    if (result == NULL) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create repr string");
        return NULL;
    }
    
    debug_print_cpuid("Exiting Register_repr successfully\n");
    return result;
}
static int Method_Register_Class(traverse)(Register *self, visitproc visit, void *arg) {
    // Si q_elements tiene una referencia a un objeto Python, la recorremos
    Py_VISIT(self->q_elements);
    return 0;
}
static int Method_Register_Class(clear)(Register *self) {
    // Eliminamos la referencia a q_elements
    Py_CLEAR(self->q_elements);
    return 0;
}


#endif