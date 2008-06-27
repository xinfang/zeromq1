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

//  Standalone application for testing video capture
//  (without use of 0MQ)

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <unicap.h>
#include <SDL.h>

#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+(((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)

int main (int argc, char *argv [])
{
    unicap_handle_t handle;
    unicap_device_t device;
    unicap_format_t format_spec;
    unicap_format_t format;
    unicap_data_buffer_t buffer;
    unicap_data_buffer_t *returned_buffer;
    
    SDL_Surface *screen;
    SDL_Surface *rgb_surface;

    int quit = 0;

    //  Open first available video capture device
    if (!SUCCESS (unicap_enumerate_devices (NULL, &device, 0)))
    {
        fprintf (stderr, "Could not enumerate devices\n");
        exit (1);
    }
    if (!SUCCESS (unicap_open (&handle, &device)))
    {
        fprintf (stderr, "Failed to open device: %s\n", device.identifier);
        exit (1);
    }
    printf( "Opened video capture device: %s\n", device.identifier );

    //  Get available RGB24 video formats and use the first one found
    unicap_void_format (&format_spec);
    format_spec.fourcc = FOURCC ('B', 'G', 'R', '3');
    if (!SUCCESS (unicap_enumerate_formats (handle, &format_spec, &format, 0)))
    {
        fprintf (stderr, "Could not enumerate video formats\n");
        exit (1);
    }
    if (!SUCCESS (unicap_set_format (handle, &format)))
    {
        fprintf (stderr, "Failed to set video format\n");
        exit (1);
    }
    printf ("Using video format: %s [%dx%d]\n", 
        format.identifier, 
        format.size.width, 
        format.size.height);


    //  Initialise SDL
    if (SDL_Init (SDL_INIT_VIDEO) < 0)
    {
        fprintf (stderr, "Failed to initialize SDL:  %s\n", SDL_GetError());
        exit (1);
    }
    screen = SDL_SetVideoMode (format.size.width, format.size.height, 32, SDL_HWSURFACE);
    if (screen == NULL) {
       fprintf (stderr, "Unable to set video mode: %s\n", SDL_GetError ());
       SDL_Quit ();
       exit (1);
    }
    
    //  Initialise image buffer
    memset (&buffer, 0, sizeof (unicap_data_buffer_t));
    buffer.data = malloc (format.buffer_size);
   
    //  Start video capture
    if (!SUCCESS (unicap_start_capture (handle)))
    {
        fprintf (stderr, "Failed to start capture on device: %s\n", device.identifier);
        SDL_Quit ();
        exit (1);
    }

    //  Display video until user closes window
    while (!quit)
    {
        SDL_Event event;

        //  Queue buffer for video capture
        if (!SUCCESS (unicap_queue_buffer (handle, &buffer)))
        {
            fprintf (stderr, "Failed to queue a buffer on device: %s\n", device.identifier);
            SDL_Quit ();
            exit (1);
        }
        //  Wait until buffer is ready
        if (!SUCCESS (unicap_wait_buffer (handle, &returned_buffer)))
        {
            fprintf (stderr, "Failed to wait for buffer on device: %s\n", device.identifier);
        }
       
        //  Create RGB surface
        rgb_surface = SDL_CreateRGBSurfaceFrom (
            buffer.data,                //  Pixel data
            format.size.width,          //  Width
            format.size.height,         //  Height
            24,                         //  Depth
            format.size.width * 3,      //  Scanline pitch
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
   
    //  Cleanup and quit
    if (!SUCCESS (unicap_stop_capture (handle)))
    {
        fprintf (stderr, "Failed to stop capture on device: %s\n", device.identifier);
    }
    if (!SUCCESS (unicap_close (handle)))
    {
        fprintf (stderr, "Failed to close the device: %s\n", device.identifier);
    }
    SDL_Quit();
    
    return 0;
}
