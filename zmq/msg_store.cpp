/*
    Copyright (c) 2007-2008 FastMQ Inc.

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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>

#include "msg_store.hpp"

zmq::msg_store_t::msg_store_t (const char *filename_, size_t filesize_) :
    filename (filename_),
    filesize (filesize_),
    file_pos (0),
    write_pos (0),
    read_pos (0),
    n_msgs (0)
{
    //  Open the message store file.
    fd = open (filename.c_str (), O_RDWR | O_CREAT, 0600);
    errno_assert (fd != -1);

    //  Enable more aggresive read-ahead optimization.
    int rc = posix_fadvise (fd, 0, filesize, POSIX_FADV_SEQUENTIAL);
    errno_assert (rc == 0);
}

zmq::msg_store_t::~msg_store_t ()
{
    int rc = close (fd);
    errno_assert (rc == 0);

    rc = unlink (filename.c_str ());
    errno_assert (rc == 0);
}

bool zmq::msg_store_t::store (message_t &msg)
{
    size_t msg_size = msg.size ();

    //  Check buffer space availability.
    //  NOTE: We always keep one byte open.
    if (buffer_space () <= sizeof msg_size + msg_size)
        return false;

    //  Write the message length.
    copy_to_file (&msg_size, sizeof msg_size);

    //  Write the message body.
    copy_to_file (msg.data (), msg.size ());

    //  Update the message counter.
    n_msgs ++;

    return true;
}

void zmq::msg_store_t::fetch (message_t &msg)
{
    //  There must be at least one message available.
    assert (n_msgs > 0);

    //  Retrieve the message size.
    size_t msg_size;
    copy_from_file (&msg_size, sizeof msg_size);

    //  Build the message.
    msg.rebuild (msg_size);
    copy_from_file (msg.data (), msg.size ());

    //  Update the message counter.
    n_msgs --;
}

bool zmq::msg_store_t::empty ()
{
    return n_msgs == 0;
}

unsigned long zmq::msg_store_t::size ()
{
    return n_msgs;
}

void zmq::msg_store_t::copy_from_file (void *buf_, size_t count_)
{
    char *ptr = (char *) buf_;
    size_t n_read, n_left = count_;

    while (n_left > 0) {

        if (read_pos != file_pos) {
            off_t offset = lseek (fd, read_pos, SEEK_SET);
            errno_assert (offset == read_pos);
            file_pos = read_pos;
        }

        if (n_left < filesize - read_pos)
            n_read = n_left;
        else
            n_read = filesize - read_pos;

        ssize_t n = read (fd, ptr, n_read);
        errno_assert (n > 0);

        ptr += n;
        file_pos += n;
        read_pos = (read_pos + n) % filesize;

        n_left -= n;
    }
}

void zmq::msg_store_t::copy_to_file (const void *buf_, size_t count_)
{
    char *ptr = (char *) buf_;
    size_t n_write, n_left = count_;

    while (n_left > 0) {

        if (write_pos != file_pos) {
            off_t offset = lseek (fd, write_pos, SEEK_SET);
            errno_assert (offset == write_pos);
            file_pos = write_pos;
        }

        if (n_left < filesize - write_pos)
            n_write = n_left;
        else
            n_write = filesize - write_pos;

        ssize_t n = write (fd, ptr, n_write);
        errno_assert (n > 0);

        ptr += n;
        file_pos += n;
        write_pos = (write_pos + n) % filesize;

        n_left -= n;
    }
}

size_t zmq::msg_store_t::buffer_space ()
{
    if (write_pos < read_pos)
        return read_pos - write_pos;

    return filesize - (write_pos - read_pos);
}
