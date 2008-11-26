*NOTE* The ZMQ cmake build is still a work in progress

*TODO* perf build not complete yet
*TODO* pyzmq build not complete yet
*TODO* jzmq build not complete yet
*TODO* issue with camera example, glib appears to be a dependency
*TODO* figure out the best way to update/change compile flags
*TODO* documentation

When using cmake, make sure the compiler is in your path or set the 
CC and CXX environment variables before you build.

For example, on AIX, 
  CC=/usr/vac/bin/xlc_r; export CC
  CXX=/usr/vac/bin/xlC_r; export CXX

To build a debug version on unix, 

  mkdir /tmp/zmq-debug-build; cd /tmp/zmq-debug-build
  cmake -D"CMAKE_INSTALL_PREFIX=${HOME}/zmq" /path/to/source
  make; make install

To build a release version on unix, 

  mkdir /tmp/zmq-release-build; cd /tmp/zmq-release-build
  cmake \
    -D"CMAKE_INSTALL_PREFIX=${HOME}/zmq" \
    -D"CMAKE_BUILD_TYPE=Release" \
    /path/to/source
  make; make install
  

To build a debug version on Windows using nmake, 

  mkdir c:\temp\zmq-debug-build
  cd c:\temp\zmq-debug-build

  cmake \
    -G"NMake Makefiles" \
    -D"CMAKE_BUILD_TYPE=Debug" \
    -D"CMAKE_INSTALL_PREFIX=%userprofile%\Desktop\zmq" \
    c:\path\to\source
  nmake & nmake install
  
To build a release version on Windows using nmake, 

  mkdir c:\temp\zmq-debug-build
  cd c:\temp\zmq-debug-build

  cmake \
    -G"NMake Makefiles" \
    -D"CMAKE_BUILD_TYPE=Release" \
    -D"CMAKE_INSTALL_PREFIX=%userprofile%\Desktop\zmq" \
    c:\path\to\source
  nmake & nmake install


To build the C, Java and Python addons, 

  -D"WITH_C=ON" -D"WITH_JAVA=ON" -D"WITH_PYTHON=ON" 

To build the examples,

  -D"WITH_CHAT=ON" -D"WITH_EXCHANGE=ON" -D"WITH_CAMERA=ON" 

To build shared libraries, 

  -D"BUILD_SHARED_LIBS=ON"
