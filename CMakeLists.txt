cmake_minimum_required(VERSION 2.4.5 FATAL_ERROR)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

project(zmq)

# build options
option(BUILD_SHARED_LIBS "Build Shared Libs?"        ON)
option(WITH_C            "Build C language binding?" OFF)
option(WITH_SCTP         "Build with SCTP protocol?" OFF)
option(WITH_PERF         "Build performance tests?" OFF)

#  By default gettime is used for time measure
set (time_measure "gettime")

# Including zmq custom cmake modules 
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# load zmq cmake macros
include(zmq_macros) 

# zmq platform checks
include(zmq_platform_checks) 

# where to install zmq software
set(ZMQ_INSTALL_BINARIES  "bin")
set(ZMQ_INSTALL_LIBRARIES "lib")
set(ZMQ_INSTALL_INCLUDES  "include")

# Descend zmq build tree and compile everything
set(zmq_subdirs libzmq zmq_server libczmq perf)

foreach(subdir ${zmq_subdirs})
  add_subdirectory("${subdir}")
endforeach(subdir ${zmq_subdirs})


MESSAGE(STATUS  "")
MESSAGE(STATUS  "")
MESSAGE(STATUS  " ********************************************************" )
MESSAGE(STATUS  "                       0MQ")
MESSAGE(STATUS  " ********************************************************" )
MESSAGE(STATUS  "   This software is distributed under the terms and " )
MESSAGE(STATUS  "   conditions of the LESSER GNU GENERAL PUBLIC LICENSE." )
MESSAGE(STATUS  "   See the file COPYING and COPYING.LESSER for the full" )
MESSAGE(STATUS  "   license text. " )
MESSAGE(STATUS  " ********************************************************" )
MESSAGE(STATUS  "")
MESSAGE(STATUS  " 0MQ install dir: ${CMAKE_INSTALL_PREFIX}" )
MESSAGE(STATUS  "")

if(WITH_ESTIMATOR)
MESSAGE(STATUS  " Utilities:" )
MESSAGE(STATUS  "   cpu frequency estimator: ${cpu_estimator} " )
MESSAGE(STATUS  "")
endif(WITH_ESTIMATOR)

MESSAGE(STATUS  " Addons:" )

MESSAGE(STATUS  "   performance tests ........ ${WITH_PERF} (${time_measure})" )

MESSAGE(STATUS  "   camera example ........... ${WITH_CAMERA}" )
MESSAGE(STATUS  "   stock exchange example ... ${WITH_EXCHANGE}" )
MESSAGE(STATUS  "   chatroom example ......... ${WITH_CHAT}" )
MESSAGE(STATUS  "")
MESSAGE(STATUS  " C language binding ......... ${WITH_C}" )
MESSAGE(STATUS  " Python language binding .... ${WITH_PYTHON}" )
MESSAGE(STATUS  " Java language binding ...... ${WITH_JAVA}" )
MESSAGE(STATUS  " SCTP capable ............... ${WITH_SCTP}" )
MESSAGE(STATUS  "")
