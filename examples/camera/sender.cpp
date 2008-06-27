/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//  Video over 0MQ, sender half.

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <unicap.h>

#include <zmq/dispatcher.hpp>
#include <zmq/api_thread.hpp>
#include <zmq/poll_thread.hpp>
#include <zmq/locator.hpp>
#include <zmq/message.hpp>
#include <zmq/wire.hpp>

#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+(((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)

int main (int argc, char *argv [])
{
    unicap_handle_t handle;
    unicap_device_t device;
    unicap_format_t format_spec;
    unicap_format_t format;
    unicap_data_buffer_t buffer;
    unicap_data_buffer_t *returned_buffer;

    if (argc != 6) {
        fprintf (stderr, "Usage: sender <locator address> <locator port> "
            "<exchange> <interface> <port>\n");
        exit (1);
    }

    //  Initialise 0MQ infrastructure
    zmq::dispatcher_t dispatcher (4);
    zmq::locator_t locator (argv [1], atoi (argv [2]));
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);
    zmq::poll_thread_t *pt = zmq::poll_thread_t::create (&dispatcher);
    int e_id = api->create_exchange (argv [3], zmq::scope_global, argv [4], 
        atoi (argv [5]), pt, 1, &pt);
    
    //  Open first available video capture device
    if (!SUCCESS (unicap_enumerate_devices (NULL, &device, 0))) {
        fprintf (stderr, "Could not enumerate devices\n");
        exit (1);
    }
    if (!SUCCESS (unicap_open (&handle, &device))) {
        fprintf (stderr, "Failed to open device: %s\n", device.identifier);
        exit (1);
    }
    printf( "Opened video capture device: %s\n", device.identifier );

    //  Get available RGB24 video formats and use the first one found
    unicap_void_format (&format_spec);
    format_spec.fourcc = FOURCC ('B', 'G', 'R', '3');
    if (!SUCCESS (unicap_enumerate_formats (handle, &format_spec, &format, 0))) {
        fprintf (stderr, "Could not enumerate video formats\n");
        exit (1);
    }
    if (!SUCCESS (unicap_set_format (handle, &format))) {
        fprintf (stderr, "Failed to set video format\n");
        exit (1);
    }
    printf ("Using video format: %s [%dx%d]\n", 
        format.identifier, 
        format.size.width, 
        format.size.height);

    //  Initialise image buffer
    memset (&buffer, 0, sizeof (unicap_data_buffer_t));
    buffer.data = (unsigned char *)malloc (format.buffer_size);
   
    //  Start video capture
    if (!SUCCESS (unicap_start_capture (handle))) {
        fprintf (stderr, "Failed to start capture on device: %s\n", device.identifier);
        exit (1);
    }

    //  Loop, sending video to defined exchange
    while (1) {
        //  Queue buffer for video capture
        if (!SUCCESS (unicap_queue_buffer (handle, &buffer))) {
            fprintf (stderr, "Failed to queue a buffer on device: %s\n", device.identifier);
            exit (1);
        }
        //  Wait until buffer is ready
        if (!SUCCESS (unicap_wait_buffer (handle, &returned_buffer))) {
            fprintf (stderr, "Failed to wait for buffer on device: %s\n", device.identifier);
        }
        //  Create ZMQ message
        zmq::message_t msg (format.buffer_size + (2 * sizeof (uint32_t)));
        unsigned char *data = (unsigned char *)msg.data();
        //  Image width in pixels
        zmq::put_uint32 (data, (uint32_t)format.size.width);
        data += sizeof (uint32_t);
        //  Image height in pixels
        zmq::put_uint32 (data, (uint32_t)format.size.height);
        data += sizeof (uint32_t);
        //  RGB24 image data
        memcpy (data, buffer.data, format.buffer_size);
        //  Send message 
        api->send (e_id, &msg);
    }
    
    return 0;
}
