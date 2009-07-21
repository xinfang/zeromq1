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
 


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <tcl.h>
#ifdef __VMS
#pragma names save
#pragma names as_is
#endif
#include <zmq.h>
#ifdef __VMS
#pragma names restore
#endif



# if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION < 4
# define TCONST
# else
# define TCONST const
# endif


typedef struct
{
    uint64_t 	dlen;
    uint32_t 	type;
    char * 	data;
}
buffer_t;



static int _create (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    void * 		obj;

    char 		tmp[16] = "-1";

    char * 		host;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if ((host = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if ((obj = zmq_create (host)) != NULL)
    {
       sprintf(tmp, "%#lx", (unsigned long) obj);
    }

    Tcl_SetStringObj(result, tmp, -1);

    return (TCL_OK);
}


static int _destroy (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    void *	 	obj;
    char * 		loc;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if ((loc = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(loc, NULL, 0);

    zmq_destroy(obj);


    Tcl_SetIntObj (result, 0L);

    return (TCL_OK);
}


static int _mask (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    int 		notifications;

    void *	 	obj;

    char *  		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 3)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if (Tcl_GetIntFromObj (interp, objv[2], &notifications) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    zmq_mask (obj, notifications);

    Tcl_SetIntObj (result, 0L);

    return (TCL_OK);
}


static int _create_exchange (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    int                 eid;

    int 		scope;
    int 		style;

    void *	 	obj;

    char *  		addr;
    char * 		name;
    char * 		locn;


    result = Tcl_GetObjResult (interp);

    if (objc != 6)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if ((name = Tcl_GetStringFromObj (objv[2], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj (interp, objv[3], &scope) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if ((locn = Tcl_GetStringFromObj (objv[4], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (*locn == '\0')
    {
       locn = NULL;
    }

    if (Tcl_GetIntFromObj (interp, objv[5], &style) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    eid = zmq_create_exchange (obj, name, scope, locn, style);

    Tcl_SetIntObj (result, (long) eid);

    return (TCL_OK);
}


static int _create_queue (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    int 		qid;

    int 		scope, hwm, lwm, swap;

    void *	 	obj;

    char * 		addr;
    char * 		name;
    char * 		locn;


    result = Tcl_GetObjResult (interp);

    if (objc != 8)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if ((name = Tcl_GetStringFromObj (objv[2], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj (interp, objv[3], &scope) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if ((locn = Tcl_GetStringFromObj (objv[4], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (*locn == '\0')
    {
       locn = NULL;
    }

    if (Tcl_GetIntFromObj (interp, objv[5], &hwm) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj (interp, objv[6], &lwm) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj (interp, objv[7], &swap) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    qid = zmq_create_queue (obj, name, scope, locn, hwm, lwm, swap);

    Tcl_SetIntObj (result, (long) qid);

    return (TCL_OK);
}


static int _bind (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    char * 		addr;

    void *	 	obj;

    char * 		ename;
    char *            	qname;

    char * 		eopts;
    char * 		qopts;


    result = Tcl_GetObjResult (interp);

    if (objc != 6)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if ((ename = Tcl_GetStringFromObj (objv[2], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if ((qname = Tcl_GetStringFromObj (objv[3], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if ((eopts = Tcl_GetStringFromObj (objv[4], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (*eopts == '\0')
    {
       eopts = NULL;
    }

    if ((qopts = Tcl_GetStringFromObj (objv[5], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (*qopts == '\0')
    {
       qopts = NULL;
    }

    zmq_bind (obj, ename, qname, eopts, qopts);

    Tcl_SetIntObj (result, 0L);

    return (TCL_OK);
}


static int _send (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    buffer_t * 		buf;

    int                 eid, block;

    void *	 	obj;

    int     		rv;

    char * 		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 5)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if (Tcl_GetIntFromObj (interp, objv[2], &eid) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[3], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    buf = (buffer_t *) strtoul(addr, NULL, 0);

    if (Tcl_GetIntFromObj (interp, objv[4], &block) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    rv = zmq_send (obj, eid, buf->data, buf->dlen, block);

    Tcl_SetIntObj (result, (long) rv);

    return (TCL_OK);
}


static int _receive (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    char 		tmp[16];

    uint64_t 		olen;
    uint32_t 		type;

    int                 qid, block;

    void *	 	obj;

    buffer_t *      	buf;

    char * 		addr;
    void * 		data;


    result = Tcl_GetObjResult (interp);

    if (objc != 3)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    obj = (void *) strtoul(addr, NULL, 0);

    if (Tcl_GetIntFromObj (interp, objv[2], &block) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }


    qid = zmq_receive (obj, &data, &olen, &type, block);

    assert ((buf = (buffer_t *) malloc (sizeof(buffer_t))));

    buf->dlen = olen;
    buf->type = type;
    buf->data = data;

    sprintf (tmp, "%#lx", (unsigned long) buf);

    Tcl_SetStringObj (result, tmp, -1);

    return (TCL_OK);
}


static int _free (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    buffer_t *	 	buf;

    char * 		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if ((buf = (buffer_t *) strtoul(addr, NULL, 0)) != NULL)
    {
       if (buf->data != NULL)
       {
          zmq_free (buf->data);
       }

       free(buf);
    }

    Tcl_SetIntObj (result, 0L);

    return (TCL_OK);

}


static int _size (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *       	result;

    long 		dlen = -1;

    buffer_t *	 	buf;

    char * 		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if ((buf = (buffer_t *) strtoul(addr, NULL, 0)) != NULL)
    {
       dlen = buf->dlen;
    }

    Tcl_SetIntObj (result, dlen);

    return (TCL_OK);
}


static int _buffer(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *         	result;

    char             	tmp[16] = "-1";

    int                	len;

    buffer_t * 		buf = NULL;


    result = Tcl_GetObjResult(interp);

    if (objc != 2)
    {
       Tcl_SetStringObj(result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj(interp, objv[1], &len) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    assert ((buf = (buffer_t *) malloc (sizeof(buffer_t))));

    buf->dlen = len;
    buf->type = 0;

    assert ((buf->data = (void *) malloc (len)));

    memset (buf->data, '\0', len);

    sprintf (tmp, "%#lx", (unsigned long) buf);


    Tcl_SetStringObj (result, tmp, -1);

    return (TCL_OK);
}


static int Zmqtcl_HaveRoom (buffer_t *buf, int off, int len)
{
    if ((off + len) > buf->dlen)
    {
       fprintf (stderr, "Insufficient space in buffer (%d, %lld)", off + len, 
           (long long int)buf->dlen);

       return (-1);
    }

    return (0);
}


static int Zmqtcl_CheckBuffer (buffer_t *buf)
{
    int 		rv = -1;

    if (buf == NULL)
    {
       fprintf (stderr, "Invalid buffer (null pointer detected)");
    }
    else
    {
       if ((buf->data == NULL) || (buf->dlen == 0))
       {
          fprintf (stderr, "No data in buffer (potentially corrupt buffer)");
       }
       else
       {
          rv = 0;
       }
    }

    return (rv);
}


# define BPL 16

static int _dump (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *        	result;

    unsigned char * 	tmp;

    int 		rv = 0;

    int 		n;

    buffer_t * 		buf = NULL;

    int 		i;
    int 		j;

    char * 		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    buf = (buffer_t *) strtoul (addr, NULL, 0);

    if (Zmqtcl_CheckBuffer (buf) == 0)
    {
       tmp = (unsigned char *) buf->data; n = buf->dlen;

       for (i = 0; i < n; i += BPL)
       {
          printf ("%04x  ", i);

          for (j = i; j < n && (j - i) < BPL; j++)
          {
             printf ("%02x ", tmp[j]);
          }

          for (; 0 != (j % BPL); j++)
          {
             printf ("   ");
          }

          printf ("  |");

          for (j = i; j < n && (j - i) < BPL; j++)
          {
             printf ("%c", (isprint (tmp[j])) ? tmp[j] : '.');
          }

          printf ("|\n");
       }
    }
    else
    {
       rv = -1;
    }

    Tcl_SetIntObj (result, (long) rv);

    return (TCL_OK);
}


static int _clear (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *        	result;

    int 		rv = 0;

    buffer_t * 		buf = NULL;

    char * 		addr;


    result = Tcl_GetObjResult (interp);

    if (objc != 2)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }


    if ((addr = Tcl_GetStringFromObj (objv[1], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    buf = (buffer_t *) strtoul (addr, NULL, 0);

    if ((rv = Zmqtcl_CheckBuffer (buf)) == 0)
    {
       memset (buf->data, '\0', buf->dlen);
    }

    Tcl_SetIntObj (result, (long) rv);

    return (TCL_OK);
}



static TCONST char *types[] = {
	"-char"		,
	"-string"	,
	"-short"	,
	"-int"		,
	"-long"		,
	"-float"	,
	"-double"	, NULL };



static int _set (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj * 		result;

    char * 		str;

    buffer_t * 		buf;

    int 		rv;

    int 		slen;
    int 		dlen;

    char * 		addr;

    int 		idx;
    int 		off;


    result = Tcl_GetObjResult (interp);

    if (objc < 5 || objc > 6)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if (Tcl_GetIndexFromObj (interp, objv[1], types, "option", TCL_EXACT, &idx) != TCL_OK)
    {
       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[2], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    buf = (buffer_t *) strtoul (addr, NULL, 0);

    if ((str = Tcl_GetStringFromObj (objv[3], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    if (Tcl_GetIntFromObj (interp, objv[4], &off) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if (objc == 6)
    {
       if (Tcl_GetIntFromObj (interp, objv[5], &dlen) == TCL_ERROR)
       {
          return (TCL_ERROR);
       }
    }
    else
    {
       dlen = -1;
    }


    if ((rv = Zmqtcl_CheckBuffer (buf)) == 0)
    {
       switch (idx)
       {
          case 0:
          case 1:
             slen = strlen (str);

	     if (dlen == -1)
	     {
	        dlen = slen;
	     }

	     if ((rv = Zmqtcl_HaveRoom (buf, off, dlen)) == 0)
	     {
	        if (dlen <= slen)
	        {
	           memcpy (&buf->data[off], str, dlen);
	        }
	        else
	        {
                   char val = (idx == 1) ? '\0' : ' ';

	           memset (&buf->data[off], val, dlen);
	           memcpy (&buf->data[off], str, slen);
	        }
	     }

	     break;

          case 2:
	     if ((rv = Zmqtcl_HaveRoom (buf, off, sizeof (short))) == 0)
	     {
	        short val = atoi (str);

	        memcpy (&buf->data[off], &val, sizeof (val));
	     }

	     break;

          case 3:
	     if ((rv = Zmqtcl_HaveRoom (buf, off, sizeof (int))) == 0)
	     {
	        int val = atoi (str);

	        memcpy (&buf->data[off], &val, sizeof (val));
	     }

	     break;

          case 4:
	     if ((rv = Zmqtcl_HaveRoom (buf, off, sizeof (long))) == 0)
	     {
	        long val = atol (str);

	        memcpy (&buf->data[off], &val, sizeof (val));
	     }

	     break;

          case 5:
	     if ((rv = Zmqtcl_HaveRoom (buf, off, sizeof (float))) == 0)
	     {
	        float val = atof (str);

	        memcpy (&buf->data[off], &val, sizeof (val));
	     }

	     break;

          case 6:
	     if ((rv = Zmqtcl_HaveRoom (buf, off, sizeof (double))) == 0)
	     {
	        double val = atof (str);

	        memcpy (&buf->data[off], &val, sizeof (val));
	     }

	     break;

          default:
	     return (TCL_ERROR);

   	     break;
       }
    }

    Tcl_SetIntObj (result, (long) rv);

    return (TCL_OK);
}


static int _get (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[])
{
    Tcl_Obj *  		result;

    buffer_t * 		buf;

    char * 		addr;

    int           	idx;
    int 		off;
    int 		len;


    result = Tcl_GetObjResult (interp);

    if (objc < 4 || objc > 5)
    {
       Tcl_SetStringObj (result, "Wrong number of arguments", -1);

       return (TCL_ERROR);
    }

    if (Tcl_GetIndexFromObj (interp, objv[1], types, "option", TCL_EXACT, &idx) != TCL_OK)
    {
       return (TCL_ERROR);
    }

    if ((addr = Tcl_GetStringFromObj (objv[2], (int *) NULL)) == NULL)
    {
       return (TCL_ERROR);
    }

    buf = (buffer_t *) strtoul (addr, NULL, 0);

    if (Tcl_GetIntFromObj (interp, objv[3], &off) == TCL_ERROR)
    {
       return (TCL_ERROR);
    }

    if (objc == 5)
    {
       if (Tcl_GetIntFromObj (interp, objv[4], &len) == TCL_ERROR)
       {
          return (TCL_ERROR);
       }
    }
    else
    {
       len = -1;
    }

    if (Zmqtcl_CheckBuffer (buf) == -1)
    {
       return (TCL_ERROR);
    }

    switch (idx)
    {
       case 0:
       case 1:
          Tcl_SetStringObj (result, &buf->data[off], len);

	  break;

       case 2:
          Tcl_SetIntObj (result, (int) (*(short *) &buf->data[off]));

	  break;

       case 3:
          Tcl_SetIntObj (result, *(int *) &buf->data[off]);

	  break;

       case 4:
          Tcl_SetLongObj (result, *(long *) &buf->data[off]);

	  break;

       case 5:
          Tcl_SetDoubleObj (result, (double) (*(float *) &buf->data[off]));

	  break;

       case 6:
          Tcl_SetDoubleObj (result, *(double *) &buf->data[off]);

	  break;

       default:
	  return (TCL_ERROR);

	  break;
    }

    return (TCL_OK);
}



static void Zmqtcl_SetVar (Tcl_Interp *interp, char *name, int val)
{
    char tmp[16];

    sprintf (tmp, "%d", val);
    Tcl_SetVar (interp, name, tmp, TCL_GLOBAL_ONLY);
}


int Tclzmq_Init (Tcl_Interp *interp)
{
    if (interp == 0)
    {
       return (TCL_ERROR);
    }

    Tcl_CreateObjCommand (interp, "ZMQ::create"          , _create         , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::destroy"         , _destroy        , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::mask"            , _mask           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::create_exchange" , _create_exchange, NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::exchange"        , _create_exchange, NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::create_queue"    , _create_queue   , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::queue"           , _create_queue   , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::bind"            , _bind           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::send"            , _send           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::receive"         , _receive        , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::free"            , _free           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::buffer"          , _buffer         , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::size"            , _size           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::dump"            , _dump           , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::clear"           , _clear          , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::set"             , _set            , NULL, NULL);
    Tcl_CreateObjCommand (interp, "ZMQ::get"             , _get            , NULL, NULL);


    Zmqtcl_SetVar (interp, "ZMQ::SCOPE_LOCAL"            , ZMQ_SCOPE_LOCAL            );
    Zmqtcl_SetVar (interp, "ZMQ::SCOPE_PROCESS"          , ZMQ_SCOPE_PROCESS          );
    Zmqtcl_SetVar (interp, "ZMQ::SCOPE_GLOBAL"           , ZMQ_SCOPE_GLOBAL           );
    Zmqtcl_SetVar (interp, "ZMQ::MESSAGE_DATA"           , ZMQ_MESSAGE_DATA           );
    Zmqtcl_SetVar (interp, "ZMQ::MESSAGE_GAP"            , ZMQ_MESSAGE_GAP            );
    Zmqtcl_SetVar (interp, "ZMQ::STYLE_DATA_DISTRIBUTION", ZMQ_STYLE_DATA_DISTRIBUTION);
    Zmqtcl_SetVar (interp, "ZMQ::STYLE_LOAD_BALANCING"   , ZMQ_STYLE_LOAD_BALANCING   );
    Zmqtcl_SetVar (interp, "ZMQ::NO_LIMIT"               , ZMQ_NO_LIMIT               );
    Zmqtcl_SetVar (interp, "ZMQ::NO_SWAP"                , ZMQ_NO_SWAP                );
    Zmqtcl_SetVar (interp, "ZMQ::TRUE"                   , ZMQ_TRUE                   );
    Zmqtcl_SetVar (interp, "ZMQ::FALSE"                  , ZMQ_FALSE                  );

    return (TCL_OK);

}
