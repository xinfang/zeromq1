# Descend zmq build tree and compile everything
set(perf_subdirs tests)

foreach(subdir ${perf_subdirs})
  add_subdirectory("${subdir}")
endforeach(subdir ${perf_subdirs})

