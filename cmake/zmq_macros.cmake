#
# Add versioned library and set install targets
#
macro(ZMQ_ADD_LIBRARY _TARGET)

  add_library(${_TARGET} ${ARGN})

  set_target_properties( ${_TARGET}
    PROPERTIES VERSION   ${ZMQ_VERSION}
               SOVERSION ${ZMQ_VERSION_MAJOR}
  )

  INSTALL(TARGETS ${_TARGET}
    RUNTIME DESTINATION ${ZMQ_INSTALL_BINARIES}
    LIBRARY DESTINATION ${ZMQ_INSTALL_LIBRARIES}
    ARCHIVE DESTINATION ${ZMQ_INSTALL_LIBRARIES}
  )
endmacro(ZMQ_ADD_LIBRARY _TARGET)

#
# Add executable and set install targets
#
macro(ZMQ_ADD_EXECUTABLE _TARGET)
  add_executable(${_TARGET} ${ARGN})

  install(TARGETS ${_TARGET}
    RUNTIME DESTINATION ${ZMQ_INSTALL_BINARIES}
    LIBRARY DESTINATION ${ZMQ_INSTALL_LIBRARIES}
  )
endmacro(ZMQ_ADD_EXECUTABLE _TARGET)

#
# compile test
#
macro(ZMQ_COMPILE_CHECK _VAR)
  
  if(CMAKE_REQUIRED_LIBRARIES)
    set(CHECK_SYMBOL_EXISTS_LIBS
      "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
  else(CMAKE_REQUIRED_LIBRARIES)
    set(CHECK_SYMBOL_EXISTS_LIBS)
  endif(CMAKE_REQUIRED_LIBRARIES)
  
  message(STATUS "Compiling ${_VAR}")
  
  try_compile(_TMP
              ${CMAKE_BINARY_DIR}
              ${PROJECT_SOURCE_DIR}/cmake/platform/${_VAR}.cpp
              COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
              CMAKE_FLAGS
                "${CHECK_SYMBOL_EXISTS_LIBS}"
              OUTPUT_VARIABLE ${_VAR}_OUTPUT
  )
  
  if(_TMP)
    set(${_VAR} 1)
    message(STATUS "Compiling ${_VAR} - found")
  else(_TMP)
    set(${_VAR})
    message(STATUS "Compiling ${_VAR} - not found")
  endif(_TMP)

endmacro(ZMQ_COMPILE_CHECK _VAR)

#
# compile and run
#
macro(ZMQ_COMPILE_AND_RUN_CHECK _VAR)

  message(STATUS "Compiling ${_VAR}")

  try_run(${_VAR} "${_VAR}_COMPILE"
    ${expressor_BINARY_DIR}
    ${expressor_SOURCE_DIR}/cmake/platform/${_VAR}.cpp
    OUTPUT_VARIABLE ${_VAR}_OUTPUT)

  if(${${_VAR}})
    message(STATUS "Compiling ${_VAR} - found")
  else(${${_VAR}})
    message(STATUS "Compiling ${_VAR} - not found")
  endif(${${_VAR}})

endmacro(ZMQ_COMPILE_AND_RUN_CHECK _VAR)
