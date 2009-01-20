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

//  Standalone application for testing video capture
//  (without use of 0MQ)

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include <unicap.h>
#include <ucil.h>
#include <SDL.h>

#define FOURCC(a,b,c,d) (unsigned int)((((unsigned int)d)<<24)+(((unsigned int)c)<<16)+(((unsigned int)b)<<8)+a)

int main (int argc, char *argv [])
{
    unicap_handle_t handle;
    unicap_device_t device;
    unicap_format_t src_format;
    unicap_format_t dest_format;
    unicap_data_buffer_t src_buffer;
    unicap_data_buffer_t dest_buffer;
    unicap_data_buffer_t *returned_buffer;
    
    SDL_Surface *screen;
    SDL_Surface *rgb_surface;

    int quit = 0;
    int conversion_found = 0;
    int index = 0;

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

    //  Find a suitable video format that we can convert to RGB24
    while (SUCCESS (unicap_enumerate_formats (handle, NULL, &src_format, index))) {
        printf ("Trying video format: %s\n", src_format.identifier);
        if (ucil_conversion_supported (FOURCC ('R', 'G', 'B', '3'), 
            src_format.fourcc)) {
            conversion_found = 1;
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

    //  Clone destination format with equal dimensions, but RGB24 colorspace
    unicap_copy_format (&dest_format, &src_format);
    strcpy (dest_format.identifier, "RGB 24bpp");
    dest_format.fourcc = FOURCC ('R', 'G', 'B', '3');
    dest_format.bpp = 24;
    dest_format.buffer_size = dest_format.size.width * dest_format.size.height * 3;
    
    //  Initialise image buffers
    memset (&src_buffer, 0, sizeof (unicap_data_buffer_t));
    src_buffer.data = malloc (src_format.buffer_size);
    src_buffer.buffer_size = src_format.buffer_size;
    memset (&dest_buffer, 0, sizeof (unicap_data_buffer_t));
    dest_buffer.data = malloc (dest_format.buffer_size);
    dest_buffer.buffer_size = dest_format.buffer_size;
    dest_buffer.format = dest_format;
 
    //  Initialise SDL
    if (SDL_Init (SDL_INIT_VIDEO) < 0) {
        fprintf (stderr, "Failed to initialize SDL:  %s\n", SDL_GetError());
        exit (1);
    }
    screen = SDL_SetVideoMode (dest_format.size.width, dest_format.size.height, 32, SDL_HWSURFACE);
    if (screen == NULL) {
       fprintf (stderr, "Unable to set video mode: %s\n", SDL_GetError ());
       SDL_Quit ();
       exit (1);
    }
  
    //  Start video capture
    if (!SUCCESS (unicap_start_capture (handle))) {
        fprintf (stderr, "Failed to start capture on device: %s\n", device.identifier);
        SDL_Quit ();
        exit (1);
    }

    //  Display video until user closes window
    while (!quit)
    {
        SDL_Event event;

        //  Queue buffer for video capture
        if (!SUCCESS (unicap_queue_buffer (handle, &src_buffer))) {
            fprintf (stderr, "Failed to queue a buffer on device: %s\n", device.identifier);
            SDL_Quit ();
            exit (1);
        }
        //  Wait until buffer is ready
        if (!SUCCESS (unicap_wait_buffer (handle, &returned_buffer))) {
            fprintf (stderr, "Failed to wait for buffer on device: %s\n", device.identifier);
            SDL_Quit ();
            exit (1);
        }
        //  Convert colorspace
        if (!SUCCESS (ucil_convert_buffer (&dest_buffer, &src_buffer))) {
            // XXX This fails sometimes for unknown reasons, just skip the frame for now
            fprintf (stderr, "Failed to convert video buffer\n");
        }
       
        //  Create RGB surface
        rgb_surface = SDL_CreateRGBSurfaceFrom (
            dest_buffer.data,           //  Pixel data
            dest_format.size.width,     //  Width
            dest_format.size.height,    //  Height
            dest_format.bpp,            //  Depth
            dest_format.size.width * 3, //  Scanline pitch
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
