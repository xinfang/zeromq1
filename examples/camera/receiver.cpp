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

//  Video over 0MQ, receiver (viewer) half.

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <SDL.h>

#include <zmq/dispatcher.hpp>
#include <zmq/api_thread.hpp>
#include <zmq/poll_thread.hpp>
#include <zmq/locator.hpp>
#include <zmq/message.hpp>
#include <zmq/wire.hpp>

int main (int argc, char *argv [])
{
    SDL_Surface *screen;
    SDL_Surface *rgb_surface;
    int image_width, image_height;

    int quit = 0;
    bool sdl_initialised = false;

    if (argc != 4) {
        fprintf (stderr, "Usage: receiver <locator address> <locator port> "
            "<exchange>\n");
        exit (1);
    }

    //  Initialise 0MQ infrastructure
    zmq::dispatcher_t dispatcher (4);
    zmq::locator_t locator (argv [1], atoi (argv [2]));
    zmq::api_thread_t *api = zmq::api_thread_t::create (&dispatcher, &locator);
    zmq::poll_thread_t *pt = zmq::poll_thread_t::create (&dispatcher);
    api->create_queue ("Q");
    bool rc = api->bind (argv [3], "Q", pt, pt);
    assert (rc);

    //  Display video until user asks to quit
    while (!quit) {
        SDL_Event event;

        //  Receive single message
        zmq::message_t msg;
        api->receive (&msg);
        //  Parse message
        unsigned char *data = (unsigned char *)msg.data();
        //  Image width in pixels
        image_width = (int)zmq::get_uint32 (data);
        data += sizeof (uint32_t);
        //  Image height in pixels
        image_height = (int)zmq::get_uint32 (data);
        data += sizeof (uint32_t);
        //  data now points to RGB24 pixel data

        if (!sdl_initialised) {
            //  Initialise SDL if not already done
            //  We need to have received at least one message, so that we
            //  know what the image size being sent is
            if (SDL_Init (SDL_INIT_VIDEO) < 0)
            {
                fprintf (stderr, "Failed to initialize SDL:  %s\n", SDL_GetError());
                exit (1);
            }
            screen = SDL_SetVideoMode (image_width, image_height, 32, SDL_HWSURFACE);
            if (screen == NULL) {
               fprintf (stderr, "Unable to set video mode: %s\n", SDL_GetError ());
               SDL_Quit ();
               exit (1);
            }

            sdl_initialised = true;
        }

        //  Create RGB surface
        rgb_surface = SDL_CreateRGBSurfaceFrom (
            data,                       //  Pixel data
            image_width,                //  Width
            image_height,               //  Height
            24,                         //  Depth
            image_width * 3,            //  Scanline pitch
            0, 0, 0, 0);                //  TODO RGBA mask 

        //  Blit surface to screen
        SDL_BlitSurface (rgb_surface, NULL, screen, NULL);
        SDL_UpdateRect (screen, 0, 0, 0, 0);
        SDL_FreeSurface (rgb_surface);

        //  Check if user asked to quit
        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
                quit = 1;
        }
    }

    //  Cleanup
    SDL_Quit ();
    
    return 0;
}
