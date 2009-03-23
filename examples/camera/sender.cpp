/*
    Copyright (c) 2007-2009 FastMQ Inc.

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
#include "./ucil.h"

#include <SDL.h>

#include <zmq.hpp>

#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+\
    (((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)

struct sender_args_t
{
    const char *camera_name;
    const char *transport_options;
    zmq::i_thread *io_thread;
    zmq::api_thread_t *api_thread;
};

void *sender_thread (void *arg)
{
    sender_args_t *sender_args = (sender_args_t*) arg;
    zmq::api_thread_t *api = sender_args->api_thread;

    unicap_handle_t handle;
    unicap_device_t device;
    unicap_format_t src_format;
    unicap_format_t dest_format;
    unicap_data_buffer_t src_buffer;
    unicap_data_buffer_t dest_buffer;
    unicap_data_buffer_t *returned_buffer;

    //  1.  Define an entry point for the messages. The name of the entry point
    //      is user-defined ("camera name"). Specify that working thread "pt"
    //      will be used to listen to new connections being created as well as
    //      to send frames to existing connections.
    int e_id = api->create_exchange (sender_args->camera_name,
        zmq::scope_global, sender_args->transport_options,
        sender_args->io_thread, 1, &sender_args->io_thread);

    // 2.   Define a local exchange for sending video data to our receiver 
    //      thread and bind it to the receiver's thread queue.
    int local_e_id = api->create_exchange ("E");
    api->bind ("E", "!local_camera", NULL, NULL);
    
    //  Open first available video capture device.
    if (!SUCCESS (unicap_enumerate_devices (NULL, &device, 0))) {
        fprintf (stderr, "Could not enumerate devices\n");
        exit (1);
    }
    if (!SUCCESS (unicap_open (&handle, &device))) {
        fprintf (stderr, "Failed to open device: %s\n", device.identifier);
        exit (1);
    }
    printf( "Opened video capture device: %s\n", device.identifier );

    //  Find a suitable video format that we can convert to RGB24.
    bool conversion_found = false;
    int index = 0;
    while (SUCCESS (unicap_enumerate_formats (handle, NULL, &src_format,
          index))) {
        printf ("Trying video format: %s\n", src_format.identifier);
        if (ucil_conversion_supported (FOURCC ('R', 'G', 'B', '3'), 
            src_format.fourcc)) {
            conversion_found = true;
            break;
        }
        index++;
    }
    if (!conversion_found) {
        fprintf (stderr, "Could not find a suitable video format\n");
        exit (1);
    }
    src_format.buffer_type = UNICAP_BUFFER_TYPE_USER;
    if (!SUCCESS (unicap_set_format (handle, &src_format))) {
        fprintf (stderr, "Failed to set video format\n");
        exit (1);
    }
    printf ("Using video format: %s [%dx%d]\n", 
        src_format.identifier, 
        src_format.size.width, 
        src_format.size.height);

    //  Clone destination format with equal dimensions, but RGB24 colorspace.
    unicap_copy_format (&dest_format, &src_format);
    strcpy (dest_format.identifier, "RGB 24bpp");
    dest_format.fourcc = FOURCC ('R', 'G', 'B', '3');
    dest_format.bpp = 24;
    dest_format.buffer_size = dest_format.size.width *
        dest_format.size.height * 3;
    
    //  Initialise image buffers.
    memset (&src_buffer, 0, sizeof (unicap_data_buffer_t));
    src_buffer.data = (unsigned char*) malloc (src_format.buffer_size);
    src_buffer.buffer_size = src_format.buffer_size;
    memset (&dest_buffer, 0, sizeof (unicap_data_buffer_t));
    dest_buffer.data = (unsigned char*) malloc (dest_format.buffer_size);
    dest_buffer.buffer_size = dest_format.buffer_size;
    dest_buffer.format = dest_format;

    //  Start video capture.
    if (!SUCCESS (unicap_start_capture (handle))) {
        fprintf (stderr, "Failed to start capture on device: %s\n",
            device.identifier);
        exit (1);
    }

    //  Loop, sending video to defined exchange.
    while (1) {

        //  Queue buffer for video capture.
        if (!SUCCESS (unicap_queue_buffer (handle, &src_buffer))) {
            fprintf (stderr, "Failed to queue a buffer on device: %s\n",
                device.identifier);
            exit (1);
        }

        //  Wait until buffer is ready.
        if (!SUCCESS (unicap_wait_buffer (handle, &returned_buffer))) {
            fprintf (stderr, "Failed to wait for buffer on device: %s\n",
                device.identifier);
            exit (1);
        }

        //  Convert colorspace.
        if (!SUCCESS (ucil_convert_buffer (&dest_buffer, &src_buffer))) {
            //  TODO: This fails sometimes for unknown reasons,
            //  just skip the frame for now.
            fprintf (stderr, "Failed to convert video buffer\n");
        }

        //  Create a message.
        zmq::message_t msg (dest_format.buffer_size + (2 * sizeof (uint32_t)));
        unsigned char *data = (unsigned char *)msg.data();

        //  Image width in pixels.
        zmq::put_uint32 (data, (uint32_t) dest_format.size.width);
        data += sizeof (uint32_t);

        //  Image height in pixels.
        zmq::put_uint32 (data, (uint32_t) dest_format.size.height);
        data += sizeof (uint32_t);

        //  RGB24 image data.
        memcpy (data, dest_buffer.data, dest_format.buffer_size);
        
        //  Make copy of message.
        zmq::message_t msg_copy;
        msg.copy_to (&msg_copy);

        //  Send message to network.
        api->send (e_id, msg);

        //  Send copy of message to local receiver.
        api->send (local_e_id, msg_copy);
    }
    
    return NULL;
}

int main (int argc, char *argv [])
{
    if (argc != 4) {
        fprintf (stderr, "Usage: sender <hostname> <camera name> "
            "<interface>\n");
        exit (1);
    }

    //  Initialise 0MQ infrastructure.

    //  1. Initialise basic infrastructure for 3 threads.
    zmq::dispatcher_t dispatcher (3);

    //  2. Initialise local locator (to connect to zmq_server).
    zmq::locator_t locator (argv [1]);

    //  3. Start one working thread (to send data to receivers).
    zmq::i_thread *io = zmq::io_thread_t::create (&dispatcher);

    //  4. Register two API threads, one for the local receiver half and another
    //     for the sender half.
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);
    zmq::api_thread_t *api_sender = zmq::api_thread_t::create (&dispatcher,
        &locator);

    //  5. Define a queue to receive messages from the sender thread. these
    //     will be used to render the 'loopback' window.
    api->create_queue ("!local_camera", zmq::scope_process);

    //  6. Start the sender thread.
    sender_args_t sender_args = {argv [2], argv [3], io, api_sender};
    pthread_t sender;
    int rc = pthread_create (&sender, NULL, sender_thread,
        (void*) &sender_args);
    assert (rc == 0);

    SDL_Surface *screen = NULL;
    SDL_Surface *rgb_surface = NULL;
    int image_width, image_height;
    bool sdl_initialised = false;
    bool quit = false;

    //  Display video until user asks to quit.
    while (!quit) {
        SDL_Event event;

        //  Receive single message.
        zmq::message_t msg;
        api->receive (&msg);

        //  Parse message.
        unsigned char *data = (unsigned char*) msg.data();
        assert (msg.size () >= sizeof (uint32_t) + sizeof (uint32_t));

        //  Image width in pixels.
        image_width = (int) zmq::get_uint32 (data);
        data += sizeof (uint32_t);

        //  Image height in pixels.
        image_height = (int) zmq::get_uint32 (data);
        data += sizeof (uint32_t);
        //  data now points to RGB24 pixel data.

        if (!sdl_initialised) {

            //  Initialise SDL if not already done.
            //  We need to have received at least one message, so that we
            //  know what the image size being sent is.
            if (SDL_Init (SDL_INIT_VIDEO) < 0)
            {
                fprintf (stderr, "Failed to initialize SDL:  %s\n",
                    SDL_GetError());
                exit (1);
            }
            screen = SDL_SetVideoMode (image_width, image_height, 32,
                SDL_HWSURFACE);
            if (screen == NULL) {
               fprintf (stderr, "Unable to set video mode: %s\n",
                   SDL_GetError ());
               SDL_Quit ();
               exit (1);
            }
            SDL_WM_SetCaption ("local", "local");

            sdl_initialised = true;
        }

        //  Create RGB surface.
        rgb_surface = SDL_CreateRGBSurfaceFrom (
            data,                       //  Pixel data
            image_width,                //  Width
            image_height,               //  Height
            24,                         //  Depth
            image_width * 3,            //  Scanline pitch
            0, 0, 0, 0);                //  TODO: RGBA mask

        //  Blit surface to screen.
        SDL_BlitSurface (rgb_surface, NULL, screen, NULL);
        SDL_UpdateRect (screen, 0, 0, 0, 0);
        SDL_FreeSurface (rgb_surface);

        //  Check if user asked to quit.
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
                quit = true;
        }
    }

    //  TODO: Send a 'stop' message to sender thread rather than killing it
    //        forcefully by terminating the main thread.

    //  Cleanup
    SDL_Quit ();
    
    return 0;
}


