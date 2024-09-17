#include "Class.h"
#ifndef __CLASS_Register_H__
#define __CLASS_Register_H__

#define Method_Register_Class(name) Register_ ## name

#include <stdint.h>

typedef struct {
    PyObject_HEAD
    
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    PyObject   q_base;     /* Almacenamiento para nuestro tipo y recuento de referencias. */
    Py_ssize_t q_maxsize;  /* el número máximo de elementos en q_elements */
    PyObject*  q_elements; /* Los elementos en la cola como una lista de Python */
} Register;

PyDoc_STRVAR(Method_Register_Class(doc), "Clase que contiene la informacion devuelta por CPUID");
static PyTypeObject Method_Register_Class(type_Class);

// Metodos Inprescindibles y porpias de Python
static void      Method_Register_Class(init_type_object) (void);
static int       Method_Register_Class(init)             (Register *self, PyObject *args, PyObject *kwds);
static PyObject *Method_Register_Class(new)              (PyTypeObject *type, PyObject *args, PyObject *kwds);
static void      Method_Register_Class(dealloc)          (Register *self);
static PyObject* Method_Register_Class(repr)             (Register *self);
static int       Method_Register_Class(traverse)         (Register *self, visitproc visit, void *arg);
static int       Method_Register_Class(clear)            (Register *self);

// Metodos propios
//static PyObject* Method_Register_Class(cpuid)              (Cpuid *self, PyObject *args);

// Metodos setter y getter de atributos:
static PyObject* Method_Register_Class(get_eax)(Register *self, void *closure);
static PyObject* Method_Register_Class(get_ebx)(Register *self, void *closure);
static PyObject* Method_Register_Class(get_ecx)(Register *self, void *closure);
static PyObject* Method_Register_Class(get_edx)(Register *self, void *closure);
static int       Method_Register_Class(set_eax)(Register *self, PyObject *value, void *closure);
static int       Method_Register_Class(set_ebx)(Register *self, PyObject *value, void *closure);
static int       Method_Register_Class(set_ecx)(Register *self, PyObject *value, void *closure);
static int       Method_Register_Class(set_edx)(Register *self, PyObject *value, void *closure);

// Definimos los métodos de la clase
static PyMethodDef Method_Register_Class(methods)[] = {
    {NULL}  // Indicador de fin de la lista de métodos
};

// atributos de la clase:
static PyGetSetDef Method_Register_Class(getsetters)[] = {
    {"eax", (getter)Method_Register_Class(get_eax), (setter)Register_set_eax, "Registro eax", NULL},
    {"ebx", (getter)Method_Register_Class(get_ebx), (setter)Register_set_ebx, "Registro ebx", NULL},
    {"ecx", (getter)Method_Register_Class(get_ecx), (setter)Register_set_ecx, "Registro ecx", NULL},
    {"edx", (getter)Method_Register_Class(get_edx), (setter)Register_set_edx, "Registro edx", NULL},
    {NULL}  // Sentinel
};

#include "Register.c"
#endif