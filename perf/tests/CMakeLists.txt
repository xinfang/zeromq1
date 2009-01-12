# Descend zmq build tree and compile everything
set(tests_subdirs zmq)

foreach(subdir ${tests_subdirs})
  add_subdirectory("${subdir}")
endforeach(subdir ${tests_subdirs})

