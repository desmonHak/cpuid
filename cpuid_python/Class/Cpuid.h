#include "Class.h"
#ifndef __CLASS_CPUID_H__
#define __CLASS_CPUID_H__


/*
 * Si pensamos en el diseño de un PyObject, tenemos al menos dos campos: PyObject.ob_type, 
 * un puntero al tipo de la instancia, y luego PyObject.ob_refcnt, la cantidad de referencias 
 * a este objeto. La instancia sabe cómo encontrar la implementación de todos los métodos 
 * buscando su propio tipo en el campo PyObject.ob_type y luego encontrando el método dado 
 * en el objeto de tipo.
 * Por ejemplo: dada la expresión de Python: repr(ob), podemos comenzar convirtiéndola en 
 * la llamada de API de C equivalente: PyObject_Repr(ob). PyObject_Repr() se reduce a la 
 * siguiente llamada:
 * PyObject* result = Py_TYPE(ob)->tp_repr(ob);
 * 
 * Al recorrer esta línea, comenzamos obteniendo una referencia al tipo de ob con Py_TYPE(). 
 * Luego, buscamos la ranura PyTypeObject.tp_repr del tipo de ob, que es la implementación 
 * repr() para ob.
 * 
 * Hay espacios para muchas de las funciones integradas, por ejemplo:
 * __getattr__: PyTypeObject.tp_getattro
 * __setattr__: PyTypeObject.tp_setattro
 * __call__: PyTypeObject.tp_call
 * y así sucesivamente.
 * 
 * Esto funciona bien para los métodos especiales de Python porque es un conjunto finito de 
 * funciones que se llaman mucho, pero los tipos pueden definir sus propios métodos. Por 
 * ejemplo, dada la clase:
 * class PythonClass:
 *     def __repr__(self):
 *         return '<PythonClass instance>'
 * 
 *     def method(self):
 *         return 'something'
 * 
 * Sabemos que hay un espacio para guardar __repr__, pero ¿qué pasa con el "method"? Este no es un 
 * método especial que CPython usa, por lo que no hay un espacio reservado en la estructura 
 * PyTypeObject. Para estos métodos de forma más libre, los tipos mantienen un objeto de 
 * diccionario de Python en el espacio PyTypeObject.tp_dict desde el nombre hasta el objeto 
 * de función. El envío a través de este diccionario es más lento que ir directamente al espacio,
 * pero es necesario para permitir estos nombres arbitrarios.
 * 
 * Al definir métodos en nuestro tipo, podemos buscar los valores de estos miembros en self tal 
 * como lo haríamos en Python.
 * 
 * Usando los espacios o diccionarios de tipo podemos registrar métodos para un tipo dado, pero los 
 * datos necesitan ser almacenados en la instancia misma. Cuando definimos una nueva clase, 
 * definimos la estructura para representar una instancia de la clase. Esta estructura necesita 
 * comenzar con un campo PyObject (nota: no un puntero, un PyObject por valor) que contiene un 
 * puntero a nuestro tipo y conteo de referencia. Comenzar nuestra estructura con un PyObject 
 * hace que sea seguro hacer conversiones hacia y desde PyObject* porque un puntero a una 
 * estructura es equivalente a un puntero a su primer miembro de acuerdo con el estándar C.
 * 
 * Después del miembro PyObject, que a menudo se llama base, definimos los datos de instancia. 
 * Los datos de instancia pueden ser una mezcla de valores Python almacenados como PyObject* o 
 * datos C regulares.
 * 
 * Por ejemplo, la siguiente estructura es para una clase con un campo de lista de Python, un 
 * campo Py_ssize_t y un campo const char*.
 * 
 * typedef struct {
 *     PyObject base;            // the base object values 
 *     PyObject* list_field;     // a pointer to our list member 
 *     Py_ssize_t ssize_t_field; // our Py_ssize_t member 
 *     const char* string_field; // our const char* member 
 * } Cpuid;
 * 
 * Ranuras sin un equivalente de Python
 * Si bien muchas ranuras tienen una correspondencia 1:1 con un método especial de Python, 
 * hay algunas ranuras adicionales que podemos controlar al escribir un tipo en C.
 * 
 * Asignación y desasignación
 * Parte de la definición de una clase es cómo asignar y desasignar instancias. Esto se 
 * controla a través de las ranuras PyTypeObject.tp_alloc y PyTypeObject.tp_free.
 * 
 * Cuando llamamos a PyTypeObject.tp_new (__new__), usamos PyTypeObject.tp_alloc 
 * para obtener la memoria para almacenarse a sí mismo. En Python, hacemos esto con una 
 * llamada a object.__new__(); sin embargo, en un tipo de extensión de C podemos elegir 
 * nuestra estrategia de asignación.
 * 
 * En casi todos los casos, deberíamos usar el asignador estándar de Python, que es el valor 
 * predeterminado para PyTypeObject.tp_alloc y PyTypeObject.tp_free. Solo use un nuevo 
 * asignador si eso es fundamental para lo que está tratando de hacer. 
 * 
 * Recolección de basura
 * Si bien CPython utiliza un sistema de conteo de referencias donde los objetos se desasignan 
 * tan pronto como su PyObject.ob_refcnt llega a 0, hay casos en los que se puede formar un 
 * ciclo entre objetos que se debe interrumpir manualmente.
 * Las clases implementadas en C pueden conectarse a esta maquinaria implementando las funciones 
 * PyTypeObject.tp_traverse y PyTypeObject.tp_clear.
 * La función PyTypeObject.tp_traverse permite que el recolector de basura toque todos 
 * los objetos a los que nuestra instancia tiene una referencia. Esto se utiliza para la 
 * detección de ciclos.
 * La función PyTypeObject.tp_clear permite que el recolector de basura interrumpa los 
 * ciclos borrando explícitamente todas las referencias en un objeto elegido.
 * 
 * Banderas
 * Hay una ranura especial llamada PyTypeObject.tp_flags que es una máscara de bits de 
 * valores booleanos sobre un tipo. Uno de los bits es Py_TPFLAGS_HAVE_GC que indica que 
 * un tipo puede contener referencias y debe participar en el recolector de basura cíclico. 
 * Otras banderas como Py_TPFLAGS_LONG_SUBCLASS marcan que una clase es una subclase de 
 * PyLongObject. Esto permite verificaciones de isinstance más rápidas contra tipos comunes.
 * Al escribir un nuevo tipo, siempre debe comenzar con Py_TPFLAGS_DEFAULT y o en 
 * las otras opciones.
 */

#define Method_Cpuid_Class(name) Cpuid_ ## name

typedef struct {
    PyObject_HEAD
    int value;  // campo para almacenar un número entero

    PyObject   q_base;     /* Almacenamiento para nuestro tipo y recuento de referencias. */
    Py_ssize_t q_maxsize;  /* el número máximo de elementos en q_elements */
    PyObject*  q_elements; /* Los elementos en la cola como una lista de Python */
} Cpuid;

PyDoc_STRVAR(Cpuid_doc, "My objects");
static PyTypeObject Cpuid_type_Class;

// Metodos Inprescindibles y porpias de Python
static void      Method_Cpuid_Class(init_type_object) (void);
static int       Method_Cpuid_Class(init)             (Cpuid *self, PyObject *args, PyObject *kwds);
static PyObject *Method_Cpuid_Class(new)              (PyTypeObject *type, PyObject *args, PyObject *kwds);
static void      Method_Cpuid_Class(dealloc)          (Cpuid *self);
static PyObject* Method_Cpuid_Class(repr)             (Cpuid *self);
static int       Method_Cpuid_Class(traverse)         (Cpuid *self, visitproc visit, void *arg);
static int       Method_Cpuid_Class(clear)            (Cpuid *self);

// Metodos propios
static PyObject* Method_Cpuid_Class(cpuid)              (Cpuid *self, PyObject *args);

// Definimos los métodos de la clase
static PyMethodDef Cpuid_methods[] = {
    {"cpuid", (PyCFunction)Method_Cpuid_Class(cpuid), METH_VARARGS, "Suma un número al valor actual"},
    {NULL}  // Indicador de fin de la lista de métodos
};

#include "Cpuid.c"
#endif
