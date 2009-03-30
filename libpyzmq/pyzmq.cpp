/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Python.h>

#include <zmq.hpp>

struct pyZMQ
{
    PyObject_HEAD
    zmq::dispatcher_t *dispatcher;
    zmq::locator_t *locator;
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

void pyZMQ_dealloc (pyZMQ *self)
{
    if (self->dispatcher) {
        delete self->dispatcher;
        self->dispatcher = NULL;
    }
    if (self->locator) {
        delete self->locator;
        self->locator = NULL;
    }

    self->ob_type->tp_free ((PyObject*) self);
}

PyObject *pyZMQ_new (PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    pyZMQ *self = (pyZMQ*) type->tp_alloc (type, 0);

    if (self) {
        self->dispatcher = NULL;
        self->locator = NULL;
        self->io_thread = NULL;
        self->api_thread = NULL;
    }

    return (PyObject*) self;
}

int pyZMQ_init (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    static const char *kwlist [] = {"host", NULL};

    char const *host = NULL;

    if (!PyArg_ParseTupleAndKeywords (args, kwdict, "s", (char**) kwlist,
          &host))
        return -1;

    self->dispatcher = new zmq::dispatcher_t (2);
    self->locator = new zmq::locator_t (host);
    self->io_thread = zmq::io_thread_t::create (self->dispatcher);
    self->api_thread = zmq::api_thread_t::create (self->dispatcher,
        self->locator);

    return 0;
}
 
PyObject *pyZMQ_mask (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    int notifications;
    static const char *kwlist [] = {"notifications"};

    if (!PyArg_ParseTupleAndKeywords (args, kwdict, "i", (char**) kwlist,
          &notifications))
        return NULL;
        
    self->api_thread->mask (notifications);
    
    Py_INCREF (Py_None);
    return Py_None;

}

PyObject *pyZMQ_create_exchange (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    char const *name = NULL;
    char const *location = NULL;
    int scope = zmq::scope_local;
    int style = zmq::style_load_balancing;

    static const char *kwlist [] = {"name", "scope", "location", 
        "style", NULL};

    if (!PyArg_ParseTupleAndKeywords (args, kwdict, "s|isi", (char**) kwlist, 
        &name, &scope, &location, &style))
        return NULL;
    
    int exchange = self->api_thread->create_exchange (name, 
        (zmq::scope_t) scope, location, self->io_thread, 1,
        &self->io_thread, (zmq::style_t) style);

    return PyInt_FromLong (exchange);
}

PyObject *pyZMQ_create_queue (pyZMQ *self, PyObject *args, PyObject *kw)
{
    char const *name = NULL;
    char const *location = NULL;
    int scope = zmq::scope_local;
    int64_t hwm = -1;
    int64_t lwm = -1;
    int64_t swap = 0;

    static const char* kwlist [] = {"name", "scope", "location", "hwm", 
        "lwm", "swap", NULL};

    if (!PyArg_ParseTupleAndKeywords (args, kw, "s|isLLL", (char**) kwlist,
          &name, &scope, &location, &hwm, &lwm, &swap)) 
        return NULL;

    int queue = self->api_thread->create_queue (name, 
        (zmq::scope_t) scope, location, self->io_thread, 1,
        &self->io_thread, hwm, lwm, swap);

    return PyInt_FromLong (queue);
}

PyObject* pyZMQ_bind (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    char const *exchange_name = NULL;
    char const *queue_name = NULL;
    char const *exchange_options = NULL;
    char const *queue_options = NULL;

    static const char *kwlist [] = {"exchange", "queue", "exchange_options", 
        "queue_options", NULL};

    if (!PyArg_ParseTupleAndKeywords (args, kwdict, "ss|ss", (char**) kwlist,
          &exchange_name, &queue_name, &exchange_options, &queue_options))
        return NULL;
    
    self->api_thread->bind (exchange_name, queue_name, self->io_thread, 
        self->io_thread, exchange_options, queue_options);

    Py_INCREF (Py_None);
    return Py_None;
}

PyObject *pyZMQ_send (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    PyObject *py_message = PyString_FromStringAndSize (NULL, 0);
    int exchange = 0;
    bool block = true;
    bool sent = false;

    static const char *kwlist [] = {"exchange", "py_message", "block", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwdict, "iSb", (char**) kwlist,
          &exchange, &py_message, &block))
        return NULL;
    
    zmq::message_t message (PyString_Size (py_message));
    memcpy (message.data (), PyString_AsString (py_message), message.size ());
    sent = self->api_thread->send (exchange, message, block);

    return PyInt_FromLong (sent ? 1 : 0);
}

PyObject *pyZMQ_receive (pyZMQ *self, PyObject *args, PyObject *kwdict)
{
    zmq::message_t message;
    bool block = true;
    static const char *kwlist [] = {"block", NULL};

    if (!PyArg_ParseTupleAndKeywords (args, kwdict, "b", (char**) kwlist,
          &block))
        return NULL;

    int queue = self->api_thread->receive (&message, block);
    return Py_BuildValue ("is#i", queue, (char*) message.data (),
        message.size (), message.type ());
}

static PyMethodDef pyZMQ_methods [] =
{
    {
        "mask",
        (PyCFunction) pyZMQ_mask,
        METH_VARARGS | METH_KEYWORDS, 
        "mask (notifications) -> None\n\n"
        "Specifies which notifications should be received.\n\n" 
        "'notifications' could be either MESSAGE_DATA or"
        "MESSAGE_GAP.\n"
    },    
    {
        "create_exchange",
        (PyCFunction) pyZMQ_create_exchange,
        METH_VARARGS | METH_KEYWORDS, 
        "create_exchange (name, scope, location) -> exchange\n\n"
        "Creates new exchange, returns exchange ID.\n\n" 
        "'name' is a string specifying the name of the new exchange.\n"
        "'scope' is either pyzmq.SCOPE_LOCAL (local exchange) or "
        "pyzmq.SCOPE_GLOBAL (global exchange).\n"
        "'location' - makes sense for global exchanges only - is network "
        "interface to use for outgoing messages from this particular exchange. "
        "The format of the argument is <nic-name>[:<port>]."
    },
    {
        "create_queue",
        (PyCFunction) pyZMQ_create_queue,
        METH_VARARGS | METH_KEYWORDS, 
        "create_queue (name, scope, location) -> queue\n\n"
        "Creates new queue, returns queue ID.\n\n" 
        "'name' is a string specifying the name of the new queue.\n"
        "'scope' is either pyzmq.SCOPE_LOCAL (local queue) or "
        "pyzmq.SCOPE_GLOBAL (global queue).\n"
        "'location' - makes sense for global queues only - is network "
        "interface to use for incoming messages to this particular queue. "
        "The format of the argument is <nic-name>[:<port>]."
    },
    {
        "bind",
        (PyCFunction) pyZMQ_bind,
        METH_VARARGS | METH_KEYWORDS, 
        "bind (exchange_name, queue_name, exchange_options, "
        "queue_options) -> None\n\n"
        "Bind an exchange to a queue."
    },
    {
        "send",
        (PyCFunction) pyZMQ_send,
        METH_VARARGS | METH_KEYWORDS, 
        "send (exchange, message, block) -> sent\n\n"
        "Send a message to the specified exchange, "
        "returns boolean specifyin if the message was sent.\n"
        "'exchange' is the id of exchange.\n"
        "'message' is message to be sent.\n"
        "'block' is either true or false.\n"
    },
    {
        "receive",
        (PyCFunction) pyZMQ_receive,
        METH_VARARGS | METH_KEYWORDS, 
        "receive (block) -> (queue, message, type)\n\n"
        "Receive a message."
    },
    {
        NULL
    }
};

static const char* pyZMQ_ZMQ_doc =  
    "0MQ messaging session\n\n"
    "Available functions:\n"
    "  create_exchange\n"
    "  create_queue\n"
    "  bind\n"
    "  send\n"
    "  receive\n\n";

static PyTypeObject pyZMQType =
{
    PyObject_HEAD_INIT (NULL)
    0,
    "libpyzmq.Zmq",            /* tp_name (This will appear in the default 
                               textual representation of our objects and 
                               in some error messages)*/
    sizeof (pyZMQ),            /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor) pyZMQ_dealloc,/* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    (char*) pyZMQ_ZMQ_doc,     /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    pyZMQ_methods,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc) pyZMQ_init,     /* tp_init */
    0,                         /* tp_alloc */
    pyZMQ_new,                 /* tp_new */
};

static PyMethodDef module_methods[] =
{
    { NULL, NULL, 0, NULL }
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

static const char* pyZMQ_doc =
    "0MQ Python Module\n\n"
    "Constructor:\n"
    "  z = libpyzmq.Zmq (host)\n"
    "  'host' is <ip-address>[:<port>] of where zmq_server is running.\n"
    "\n"
    "Available functions:\n"
    "  create_exchange\n"
    "  create_queuee\n"
    "  bind\n"
    "  send\n"
    "  receive\n"
    "\n"
    "For more information see http://www.zeromq.org.\n"
    "\n"
    "0MQ is distributed under GNU Lesser General Public License v3\n";

PyMODINIT_FUNC initlibpyzmq (void)
{
    if (PyType_Ready (&pyZMQType) < 0)
        return;

    PyObject *m = Py_InitModule3 ("libpyzmq", module_methods,
        (char*) pyZMQ_doc);
    if (!m)
        return;

    Py_INCREF (&pyZMQType);

    PyModule_AddObject (m, "Zmq", (PyObject*) &pyZMQType);
    
    PyObject *d = PyModule_GetDict (m);

    PyObject *t = PyInt_FromLong (zmq::scope_local);
    PyDict_SetItemString (d, "SCOPE_LOCAL", t);
    Py_DECREF (t);

    t = PyInt_FromLong (zmq::scope_global);
    PyDict_SetItemString (d, "SCOPE_GLOBAL", t);
    Py_DECREF (t);
    
    t = PyInt_FromLong (zmq::message_data);
    PyDict_SetItemString (d, "MESSAGE_DATA", t);
    Py_DECREF (t);
    
    t = PyInt_FromLong (zmq::message_gap);
    PyDict_SetItemString (d, "MESSAGE_GAP", t);
    Py_DECREF (t);

    t = PyInt_FromLong (zmq::style_data_distribution);
    PyDict_SetItemString (d, "STYLE_DATA_DISTRIBUTION", t);
    Py_DECREF (t);

    t = PyInt_FromLong (zmq::style_load_balancing);
    PyDict_SetItemString (d, "STYLE_LOAD_BALANCING", t);
    Py_DECREF (t);

    t = PyInt_FromLong (zmq::no_limit);
    PyDict_SetItemString (d, "NO_LIMIT", t);
    Py_DECREF (t);

    t = PyInt_FromLong (zmq::no_swap);
    PyDict_SetItemString (d, "NO_SWAP", t);
    Py_DECREF (t);

    
}
