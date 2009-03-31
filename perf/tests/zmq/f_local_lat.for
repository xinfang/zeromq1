!
!     Copyright (c) 2007-2009 FastMQ Inc.
!
!     This file is part of 0MQ.
!
!     0MQ is free software; you can redistribute it and/or modify it under
!     the terms of the Lesser GNU General Public License as published by
!     the Free Software Foundation; either version 3 of the License, or
!     (at your option) any later version.
!
!     0MQ is distributed in the hope that it will be useful,
!     but WITHOUT ANY WARRANTY; without even the implied warranty of
!     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
!     Lesser GNU General Public License for more details.
!
!     You should have received a copy of the Lesser GNU General Public License
!     along with this program.  If not, see <http://www.gnu.org/licenses/>.
!

      program LOCAL LAT

      implicit none

      parameter 			ZMQ$_SCOPE_LOCAL  = 0
      parameter 			ZMQ$_SCOPE_GLOBAL = 1

      external 				ZMQ$CREATE
      external 				ZMQ$CREATE_EXCHANGE
      external 				ZMQ$CREATE_QUEUE
      external 				ZMQ$BIND
      external 				ZMQ$SEND
      external 				ZMQ$RECEIVE
      external 				ZMQ$DESTROY

      external 				lib$get_vm, lib$free_vm

      integer*4 			ZMQ$CREATE
      integer*4 			ZMQ$CREATE_EXCHANGE
      integer*4 			ZMQ$CREATE_QUEUE
      integer*4 			ZMQ$BIND
      integer*4 			ZMQ$SEND
      integer*4 			ZMQ$RECEIVE
      integer*4 			ZMQ$DESTROY

      integer*4 			lib$get_vm, lib$free_vm

      real*4 				t0
      real*4 				t1

      integer*4 			okay

      integer*4 			nmsg

      integer*4 			ilen
      integer*4 			ibuf
      integer*4 			olen
      integer*4 			obuf

      integer*4 			ffn

      character*60   			host
      character*80 			args

      integer*4 			i

      integer*4    			obj
      integer*4 			eid



      call lib$get_foreign(args)

      read(args, *) host, ilen, nmsg


!     In F90 we could just use the allocate statement here, but let's do it the
!     old way...
!
!     Strictly speaking we don't even need to use dynamic allocation.
!
      okay = lib$get_vm(ilen, ibuf)

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if


      okay = ZMQ$CREATE(%descr(host), obj)

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

      okay = ZMQ$CREATE_EXCHANGE(%val(obj), %descr('EL'), %val(ZMQ$_SCOPE_LOCAL), %val(0), eid)

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

      okay = ZMQ$CREATE_QUEUE(%val(obj), %descr('QL'), %val(ZMQ$_SCOPE_LOCAL), %val(0))

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

      okay = ZMQ$BIND(%val(obj), %descr('EL'), %descr('QG'))

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

      okay = ZMQ$BIND(%val(obj), %descr('EG'), %descr('QL'))

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

!     Note that this call is accurate to 0.01s, so should be okay for timing
!     purposes...
!
      t0 = secnds(0.0e+00)

      do i = 1, nmsg
         okay = ZMQ$SEND(%val(obj), %val(eid), %val(ibuf), %val(ilen), %val(0))

         if (.not. okay) then
            call lib$stop(%val(okay))
         end if

         okay = ZMQ$RECEIVE(%val(obj), obuf, olen, ffn)

         if (.not. okay) then
            call lib$stop(%val(okay))
         end if

         if (ffn .ne. 0) then
	    call freeit(%val(ffn), obuf)
         end if
      end do

      t1 = secnds(0.0e+00)

      print *, 'Latency = ', (t1 - t0) / (nmsg * 2.0) * 1.0e+06, 'us'


      okay = ZMQ$DESTROY(%val(obj))

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if

      okay = lib$free_vm(ilen, ibuf)

      if (.not. okay) then
         call lib$stop(%val(okay))
      end if


      end

!
!     --------------------------------------------------------------------------
!

!     Another option would be to use lib$callg() I suppose, but this method may
!     be a little faster and it seems to work... (it's been a while since I did
!     anything in FORTRAN).

      subroutine freeit(func, addr)

      implicit none

      integer*4 		addr

      external 			func

      call func(%val(addr))
      return

      end

