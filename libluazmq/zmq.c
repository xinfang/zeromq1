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
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#ifdef __VMS
#include <time.h>
#else
#include <sys/time.h>
#endif
#include <assert.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "zmq.h"


typedef struct
{
    uint64_t    dlen;
    uint32_t    type;
    char *      data;
}
buffer_t;



typedef enum
{
    z_char, z_string, z_short, z_int, z_long, z_float, z_double
}
types_t;



static int l_create (lua_State *L)
{
    const char * 	host = luaL_checkstring (L, 1);

    void *              obj;


    obj = zmq_create (host);
    lua_pushlightuserdata (L, obj);

    return (1);
}


static int l_destroy (lua_State *L)
{
    void *              obj;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    zmq_destroy (obj);

    return (0);
}


static int l_mask (lua_State *L)
{
    int                 notifications = luaL_checkint (L, 2);

    void *              obj;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    zmq_mask (obj, notifications);

    return (0);
}


static int l_create_exchange (lua_State *L)
{
    int                 eid;

    int                 scope = luaL_checkint (L, 3);
    int                 style = luaL_checkint (L, 5);

    void *              obj;

    const char *        name = luaL_checkstring (L, 2);
    const char *      	locn = luaL_checkstring (L, 4);


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    if (*locn == '\0')
    {
       locn = NULL;
    }

    eid = zmq_create_exchange (obj, name, scope, locn, style);

    lua_pushinteger (L, eid);

    return (1);
}


static int l_create_queue (lua_State *L)
{
    int                 qid;

    int                 scope, hwm, lwm, swap;

    void *              obj;

    const char *    	name = luaL_checkstring (L, 2);
    const char *    	locn = luaL_checkstring (L, 4);


    scope = luaL_checkinteger (L, 3);

    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    hwm = luaL_checkinteger (L, 5);
    lwm = luaL_checkinteger (L, 6);

    if (*locn == '\0')
    {
       locn = NULL;
    }

    swap = luaL_checkinteger (L, 7);

    qid = zmq_create_queue (obj, name, scope, locn, hwm, lwm, swap);

    lua_pushinteger (L, qid);

    return (1);
}


static int l_bind (lua_State *L)
{
    void *              obj;

    const char *     	ename;
    const char *     	qname;

    const char *      	eopts;
    const char *     	qopts;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    ename = luaL_checkstring (L, 2);
    qname = luaL_checkstring (L, 3);

    eopts = luaL_checkstring (L, 4);
    qopts = luaL_checkstring (L, 5);

    if (*eopts == '\0')
    {
       eopts = NULL;
    }

    if (*qopts == '\0')
    {
       qopts = NULL;
    }

    zmq_bind (obj, ename, qname, eopts, qopts);

    return (0);
}


static int l_send (lua_State *L)
{
    void *              obj;

    buffer_t *          buf;

    int                 eid, block;

    int                 rv;

    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    eid = luaL_checkinteger (L, 2);

    luaL_checktype (L, 3, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 3);

    luaL_checktype (L, 4, LUA_TBOOLEAN);
    block = lua_toboolean (L, 4) ? ZMQ_TRUE : ZMQ_FALSE;

    rv = zmq_send (obj, eid, buf->data, buf->dlen, block);
    lua_pushboolean (L, rv);

    return 1;
}


static int l_receive (lua_State *L)
{
    uint64_t            olen;
    uint32_t            type;

    int                 qid, block;

    void *              obj;

    buffer_t *          buf;

    void *              data;

    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    obj = lua_touserdata (L, 1);

    luaL_checktype (L, 2, LUA_TBOOLEAN);
    block = lua_toboolean (L, 2) ? ZMQ_TRUE : ZMQ_FALSE;

    qid = zmq_receive (obj, &data, &olen, &type, block);

    buf = malloc (sizeof (buffer_t));
    assert (buf != NULL);

    buf->dlen = olen;
    buf->type = type;
    buf->data = data;

    lua_pushlightuserdata (L, buf);

    return (1);
}


static int l_free (lua_State *L)
{
    buffer_t *          buf;

    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    if (buf != NULL)
    {
       if (buf->data != NULL)
       {
          zmq_free (buf->data);
       }

       free(buf);
    }

    return (0);
}


static int l_buffer (lua_State *L)
{
    int                 len = luaL_checkinteger (L, 1);

    buffer_t *          buf;


    buf = malloc (sizeof (buffer_t));
    assert (buf != NULL);

    buf->dlen = len;
    buf->type = 0;

    buf->data = malloc (buf->dlen);
    assert (buf->data != NULL);

    memset (buf->data, '\0', len);

    lua_pushlightuserdata (L, buf);

    return (1);
}


static int l_size (lua_State *L)
{
    buffer_t *          buf;

    int 		dlen = -1;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    if (buf != NULL)
    {
       dlen = buf->dlen;
    }

    lua_pushinteger (L, dlen);

    return (1);
}


static int b_check (buffer_t *buf)
{
    if (buf == NULL)
    {
       return (-1);
    }

    if ((buf->data == NULL) || (buf->dlen == 0))
    {
       return (-1);
    }

    return (0);
}


# define BPL 16

static int l_dump (lua_State *L)
{
    unsigned char *     tmp;

    int                 rv = 0;

    int                 n;

    buffer_t *          buf;

    int                 i;
    int                 j;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    if ((rv = b_check (buf)) == 0)
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

    lua_pushinteger (L, rv);

    return (1);
}


static int l_clear (lua_State *L)
{
    buffer_t *          buf;

    int                 rv = 0;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    if ((rv = b_check (buf)) == 0)
    {
       memset (buf->data, '\0', buf->dlen);
    }

    lua_pushinteger (L, rv);

    return (1);
}


static int b_room (buffer_t *buf, int off, int len)
{
    return (((off + len) > buf->dlen) ? -1 : 0);
}


static int l_set (lua_State *L)
{
    buffer_t *          buf;

    int                 rv;

    int                 off;
    int 		idx;


    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    idx = luaL_checkinteger (L, 3);
    off = luaL_checkinteger (L, 4);

    if ((rv = b_check (buf)) == 0)
    {
       switch (idx)
       {
          case z_char:
          case z_string:
          {
             const char *str = luaL_checkstring (L, 2);

             int dlen;
             int slen;

             slen = strlen (str);

             if ((dlen = luaL_checkinteger (L, 5)) == -1)
             {
                dlen = slen;
             }

             if ((rv = b_room (buf, off, dlen)) == 0)
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
          }
          case z_short:
             if ((rv = b_room (buf, off, sizeof (short))) == 0)
             {
                short val = luaL_checkinteger (L, 2);

                memcpy (&buf->data[off], &val, sizeof (val));
             }

             break;

          case z_int:
             if ((rv = b_room (buf, off, sizeof (int))) == 0)
             {
                int val = luaL_checkinteger (L, 2);

                memcpy (&buf->data[off], &val, sizeof (val));
             }

             break;

          case z_long:
             if ((rv = b_room (buf, off, sizeof (long))) == 0)
             {
                long val = luaL_checkinteger (L, 2);

                memcpy (&buf->data[off], &val, sizeof (val));
             }

             break;

          case z_float:
             if ((rv = b_room (buf, off, sizeof (float))) == 0)
             {
                float val = luaL_checknumber (L, 2);

                memcpy (&buf->data[off], &val, sizeof (val));
             }

             break;

          case z_double:
             if ((rv = b_room (buf, off, sizeof (double))) == 0)
             {
                double val = luaL_checknumber (L, 2);

                memcpy (&buf->data[off], &val, sizeof (val));
             }

             break;

          default:
             rv = -1;	/* Error handling leaves room for improvement */

             break;
       }
    }

    lua_pushinteger (L, rv);

    return (1);
}


static int l_get (lua_State *L)
{
    buffer_t *          buf;

    int                 idx;
    int                 off;
    int                 len;

    luaL_checktype (L, 1, LUA_TLIGHTUSERDATA);
    buf = lua_touserdata (L, 1);

    idx = luaL_checkinteger (L, 2);
    off = luaL_checkinteger (L, 3);
    len = luaL_checkinteger (L, 4);

    if (b_check (buf) == -1)
    {
       lua_pushnil (L);
    }
    else
    {
       switch (idx)
       {
          case z_char:
             lua_pushlstring (L, &buf->data[off], len);

             break;

          case z_string:
             lua_pushstring (L, &buf->data[off]);

             break;

          case z_short:
             lua_pushinteger (L, (int) (*(short *) &buf->data[off]));

             break;

          case z_int:
             lua_pushinteger (L, (*(int *) &buf->data[off]));

             break;

          case z_long:
             lua_pushinteger (L, (int) (*(long *) &buf->data[off]));

             break;

          case z_float:
             lua_pushnumber (L, (double) (*(float *) &buf->data[off]));

             break;

          case z_double:
             lua_pushnumber (L, (*(double *) &buf->data[off]));

             break;

          default:
             lua_pushnil (L);

             break;
       }
    }

    return (1);
}


static int l_sleep (lua_State *L)
{
    int 		nsec = luaL_checkinteger(L, 1);

    sleep (nsec);

    return (0);
}


static int l_now (lua_State *L)
{
    struct timeval          tv;

    gettimeofday(&tv, NULL);

    lua_pushnumber (L, (tv.tv_sec * 1.0e+06 + tv.tv_usec));

    return (1);
}


static const struct luaL_Reg zmq[] = {
    	{"create"          , l_create         },
    	{"destroy"         , l_destroy        },
    	{"mask"            , l_mask           },
    	{"create_exchange" , l_create_exchange},
    	{"exchange"        , l_create_exchange},
    	{"create_queue"    , l_create_queue   },
    	{"queue"           , l_create_queue   },
    	{"bind"            , l_bind           },
    	{"send"            , l_send           },
    	{"receive"         , l_receive        },
    	{"free"            , l_free           },
    	{"buffer"          , l_buffer         },
    	{"size"            , l_size           },
    	{"dump"            , l_dump           },
    	{"clear"           , l_clear          },
    	{"set"             , l_set            },
    	{"get"             , l_get            },
	{"sleep" 	   , l_sleep 	      },
	{"now" 	           , l_now 	      },
	{ NULL, NULL }
	};


#define SETVAR(name, val) { \
    lua_pushinteger (L, (val)); lua_setfield (L, -2, (name)); \
    }


int luaopen_libluazmq (lua_State *L)
{
    luaL_register (L, "zmq", zmq);

    SETVAR("SCOPE_LOCAL", ZMQ_SCOPE_LOCAL)
    SETVAR("SCOPE_PROCESS", ZMQ_SCOPE_PROCESS)
    SETVAR("SCOPE_GLOBAL", ZMQ_SCOPE_GLOBAL)
    SETVAR("MESSAGE_DATA", ZMQ_MESSAGE_DATA)
    SETVAR("MESSAGE_GAP", ZMQ_MESSAGE_GAP)
    SETVAR("STYLE_DATA_DISTRIBUTION", ZMQ_STYLE_DATA_DISTRIBUTION)
    SETVAR("STYLE_LOAD_BALANCING", ZMQ_STYLE_LOAD_BALANCING)
    SETVAR("NO_LIMIT", ZMQ_NO_LIMIT)
    SETVAR("NO_SWAP", ZMQ_NO_SWAP)

    SETVAR("char", z_char)
    SETVAR("string", z_string)
    SETVAR("short", z_short)
    SETVAR("int", z_int)
    SETVAR("long", z_long)
    SETVAR("float", z_float)
    SETVAR("double", z_double)

    return (0);
}
