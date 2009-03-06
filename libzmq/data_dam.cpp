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

#ifdef ZMQ_HAVE_DATA_DAM
#include <zmq/platform.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef ZMQ_HAVE_WINDOWS
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <string.h>

#include <zmq/data_dam.hpp>
#include <zmq/formatting.hpp>
#include <zmq/platform.hpp>

zmq::atomic_counter_t zmq::data_dam_t::counter;

zmq::data_dam_t::data_dam_t (size_t filesize_, size_t block_size_) :
    filesize (filesize_),
    file_pos (0),
    write_pos (0),
    read_pos (0),
    n_msgs (0),
    block_size (block_size_),
    write_buf_start_addr (0)
{
    assert (filesize > 0);
    assert (block_size > 0);

    buf1 = new char [block_size];
    assert (buf1);

    buf2 = new char [block_size];
    assert (buf2);

    read_buf = write_buf = buf1;

    //  Get process ID.
#ifdef ZMQ_HAVE_WINDOWS
    int pid = GetCurrentThreadId ();
#else
    pid_t pid = getpid ();
#endif

    //  Create unique file name.
    char buf [256];
    zmq_snprintf (buf, sizeof buf, "zeromq_swp.%u.%u",
        (unsigned) pid, counter.inc (1));
    filename = buf;

    //  Open the backing file.
#ifdef ZMQ_HAVE_WINDOWS
    fd = _open (filename.c_str (), _O_RDWR | _O_CREAT, 0600);
#else
    fd = open (filename.c_str (), O_RDWR | O_CREAT, 0600);
#endif
    errno_assert (fd != -1);

#ifdef ZMQ_HAVE_LINUX
    //  Enable more aggresive read-ahead optimization.
    int rc = posix_fadvise (fd, 0, filesize, POSIX_FADV_SEQUENTIAL);
    errno_assert (rc == 0);
#endif
}

zmq::data_dam_t::~data_dam_t ()
{
    delete [] buf1;
    delete [] buf2;

#ifdef ZMQ_HAVE_WINDOWS
    int rc = _close (fd);
#else
    int rc = close (fd);
#endif
    errno_assert (rc == 0);

#ifdef ZMQ_HAVE_WINDOWS
    rc = _unlink (filename.c_str ());
#else
    rc = unlink (filename.c_str ());
#endif
    errno_assert (rc == 0);
}

bool zmq::data_dam_t::store (raw_message_t *msg)
{
    size_t msg_size = raw_message_size (msg);

    //  Check buffer space availability.
    //  NOTE: We always keep one byte open.
    if (buffer_space () <= sizeof msg_size + msg_size)
        return false;

    //  Write the message length.
    copy_to_file (&msg_size, sizeof msg_size);

    //  Write the message body.
    copy_to_file (raw_message_data (msg), msg_size);

    //  Destroy the message.
    raw_message_destroy (msg);

    //  Update the message counter.
    n_msgs ++;

    return true;
}

void zmq::data_dam_t::fetch (raw_message_t *msg)
{
    //  There must be at least one message available.
    assert (n_msgs > 0);

    //  Retrieve the message size.
    size_t msg_size;
    copy_from_file (&msg_size, sizeof msg_size);

    //  Build the message.
    raw_message_init (msg, msg_size);
    copy_from_file (raw_message_data (msg), msg_size);

    //  Update the message counter.
    n_msgs --;
}

bool zmq::data_dam_t::empty ()
{
    return n_msgs == 0;
}

unsigned long zmq::data_dam_t::size ()
{
    return n_msgs;
}

static size_t min2 (size_t a, size_t b)
{
    if (a < b)
        return a;
    return b;
}

static size_t min3 (size_t a, size_t b, size_t c)
{
    if (a < b) {
        if (a < c)
            return a;
        else
            return c;
    }
    else if (b < c)
        return b;
    else
        return c;
}
void zmq::data_dam_t::copy_from_file (void *buf_, size_t count_)
{
    char *ptr = (char *) buf_;
    size_t n, n_left = count_;

    while (n_left > 0) {

        n = min3 (n_left, filesize - read_pos,
            block_size - read_pos % block_size);

        memcpy (ptr, &read_buf [read_pos % block_size], n);
        ptr += n;

        read_pos = (read_pos + n) % filesize;
        if (read_pos % block_size == 0) {
            if (read_pos / block_size == write_pos / block_size)
                read_buf = write_buf;
            else
                fill_read_buf ();
        }

        n_left -= n;
    }
}

void zmq::data_dam_t::copy_to_file (const void *buf_, size_t count_)
{
    char *ptr = (char *) buf_;
    size_t n, n_left = count_;

    while (n_left > 0) {

        n = min3 (n_left, filesize - write_pos,
            block_size - write_pos % block_size);

        memcpy (&write_buf [write_pos % block_size], ptr, n);
        ptr += n;

        write_pos = (write_pos + n) % filesize;
        if (write_pos % block_size == 0) {

            save_write_buf ();
            write_buf_start_addr = write_pos;

            if (write_buf == read_buf) {
                if (read_buf == buf2)
                    write_buf = buf1;
                else
                    write_buf = buf2;
            }
        }

        n_left -= n;
    }
}

void zmq::data_dam_t::fill_read_buf ()
{
    if (file_pos != read_pos) {
#ifdef ZMQ_HAVE_WINDOWS
        off_t offset = _lseek (fd, read_pos, SEEK_SET);
#else
        off_t offset = lseek (fd, read_pos, SEEK_SET);
#endif
        errno_assert (offset == read_pos);
        file_pos = read_pos;
    }

    size_t i = 0;
    size_t n = min2 (block_size, filesize - read_pos);

    while (i < n) {
#ifdef ZMQ_HAVE_WINDOWS
        int rc = _read (fd, &read_buf [i], n - i);
#else
        ssize_t rc = read (fd, &read_buf [i], n - i);
#endif
        errno_assert (rc > 0);
        i += rc;
    }

    file_pos += n;
}

void zmq::data_dam_t::save_write_buf ()
{
    if (file_pos != write_buf_start_addr) {
#ifdef ZMQ_HAVE_WINDOWS
        off_t offset = _lseek (fd, write_buf_start_addr, SEEK_SET);
#else
        off_t offset = lseek (fd, write_buf_start_addr, SEEK_SET);
#endif
        errno_assert (offset == write_buf_start_addr);
        file_pos = write_buf_start_addr;
    }

    size_t i = 0;
    size_t n = min2 (block_size, filesize - write_buf_start_addr);

    while (i < n) {
#ifdef ZMQ_HAVE_WINDOWS
        int rc = _write (fd, &write_buf [i], n - i);
#else
        ssize_t rc = write (fd, &write_buf [i], n - i);
#endif
        errno_assert (rc > 0);
        i += rc;
    }

    file_pos += n;
}

size_t zmq::data_dam_t::buffer_space ()
{
    if (write_pos < read_pos)
        return read_pos - write_pos;

    return filesize - (write_pos - read_pos);
}

#endif
