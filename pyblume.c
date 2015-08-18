#include <Python.h>

#include "blume.h"

typedef struct {
    PyObject_HEAD
    struct blume *bf;
} pyblume_FilterObject;


static PyObject *pyblume_CorruptException;
static PyObject *pyblume_FullException;
static PyObject *pyblume_LockedException;


static void
Filter_dealloc(pyblume_FilterObject* self)
{
    if (self->bf) {
        blume_close(&self->bf);
    }

    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Filter_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    pyblume_FilterObject *self;

    self = (pyblume_FilterObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->bf = NULL;
    }

    return (PyObject *)self;
}

static int
Filter_init(pyblume_FilterObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"max_file_size", "p", "path", NULL};
    unsigned PY_LONG_LONG max_file_size;
    double p;
    const char *path;

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "Kds", kwlist,
                                      &max_file_size, &p,
                                      &path))
        return -1;

    if (self->bf) {
        blume_close(&self->bf);
    }

    blume_error err = blume_create(&self->bf, max_file_size, p, path);
    if (err != BLUME_ERROR_SUCCESS) {
        return -1;
    }

    return 0;
}

static PyObject *
Filter_close(pyblume_FilterObject* self)
{
    if (self->bf) {
        blume_close(&self->bf);
    }

    Py_RETURN_NONE;
}

static PyObject *
Filter_add(pyblume_FilterObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"key", NULL};
    const char *key;
    int len;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#", kwlist,
                                      &key, &len))
        return NULL;

    if (!self->bf) {
        return NULL;
    }

    blume_error err = blume_add(self->bf, key, len);
    if (err != BLUME_ERROR_SUCCESS) {
        if (err == BLUME_ERROR_FULL) {
            PyErr_SetString(pyblume_FullException, "Filter is full and cannot be expanded further");
        }
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *
Filter_check(pyblume_FilterObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"key", NULL};
    const char *key;
    int len;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#", kwlist,
                                      &key, &len))
        return NULL;

    if (!self->bf) {
        return NULL;
    }

    int hit = blume_check(self->bf, key, len);
    if (hit) {
        Py_RETURN_TRUE;
    } else {
        Py_RETURN_FALSE;
    }
}

static PyMethodDef Filter_methods[] = {
    {"close", (PyCFunction)Filter_close, METH_NOARGS,
     "Close the bloom filter and free any memory used"
    },
    {"add", (PyCFunction)Filter_add, METH_VARARGS | METH_KEYWORDS,
     "Add a key to the bloom filter"
    },
    {"check", (PyCFunction)Filter_check, METH_VARARGS | METH_KEYWORDS,
     "Check if a key is in the filter"
    },
    {NULL}  /* Sentinel */
};

static int
Filter_contains(pyblume_FilterObject *self, PyObject *key)
{
    if (!PyString_Check(key)) {
        return -1;
    }
    if (!self->bf) {
        return -1;
    }
    return blume_check(self->bf,
                       PyString_AsString(key),
                       PyString_Size(key));
}

static Py_ssize_t
Filter_length(pyblume_FilterObject *self)
{
    if (!self->bf) {
        return -1;
    }
    return blume_length(self->bf);
}

static PySequenceMethods Filter_sequence = {
    (Py_ssize_t (*)(PyObject *o))Filter_length, /*sq_length*/
    NULL,                 /*sq_concat*/
    NULL,                 /*sq_repeat*/
    NULL,                 /*sq_item*/
    NULL,                 /*sq_slice*/
    NULL,                 /*sq_ass_item*/
    NULL,                 /*sq_ass_slice*/
    (objobjproc)Filter_contains, /*sq_contains*/
};

static PyTypeObject pyblume_FilterType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyblume.Filter",          /*tp_name*/
    sizeof(pyblume_FilterObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Filter_dealloc,/*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    &Filter_sequence,          /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Filter objects",          /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Filter_methods,            /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Filter_init,     /* tp_init */
    0,                         /* tp_alloc */
    Filter_new,                /* tp_new */
};

static PyObject *pyblume_open(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    unsigned char for_write = 0;
    const char *path;
    static char *kwlist[] = {"path", "for_write", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "s|b", kwlist,
                                      &path, &for_write)) {
        return NULL;
    }

    pyblume_FilterObject *self = (pyblume_FilterObject *)PyObject_New(pyblume_FilterObject, &pyblume_FilterType);
    blume_error err = blume_open(&self->bf, path, !!for_write);
    if (err != BLUME_ERROR_SUCCESS) {
        Py_DECREF(self);
        if (err == BLUME_ERROR_LOCKED) {
            PyErr_SetString(pyblume_LockedException, "Cannot open for writing -- file is already opened");
        } else if (err == BLUME_ERROR_OUT_OF_MEMORY) {
            PyErr_SetString(PyExc_MemoryError, "Cannot open filter -- memory");
        } else {
            PyErr_SetString(PyExc_IOError, "Cannot open filter -- IO");
        }
        return NULL;
    }

    err = blume_validate(self->bf);
    if (err != BLUME_ERROR_SUCCESS) {
        Py_DECREF(self);
        PyErr_SetString(pyblume_CorruptException, "The filter is corrupt");
        return NULL;
    }

    return (PyObject *) self;
}


static PyMethodDef pyblume_methods[] = {
    {"open", (PyCFunction)pyblume_open, METH_VARARGS | METH_KEYWORDS, "Open a bloom filter from the filesystem"},
    {NULL}  /* Sentinel */
};


#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initpyblume(void)
{
    PyObject* m;

    pyblume_FilterType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pyblume_FilterType) < 0)
        return;

    m = Py_InitModule3("pyblume", pyblume_methods,
                       "A module implementing a fast scalable bloom filter.");

    pyblume_FullException = PyErr_NewException("pyblume.FullException", NULL, NULL);
    Py_INCREF(pyblume_FullException);
    PyModule_AddObject(m, "FullException", pyblume_FullException);

    pyblume_CorruptException = PyErr_NewException("pyblume.CorruptException", NULL, NULL);
    Py_INCREF(pyblume_CorruptException);
    PyModule_AddObject(m, "CorruptException", pyblume_CorruptException);

    pyblume_LockedException = PyErr_NewException("pyblume.LockedException", NULL, NULL);
    Py_INCREF(pyblume_LockedException);
    PyModule_AddObject(m, "LockedException", pyblume_LockedException);

    Py_INCREF(&pyblume_FilterType);
    PyModule_AddObject(m, "Filter", (PyObject *)&pyblume_FilterType);
}
