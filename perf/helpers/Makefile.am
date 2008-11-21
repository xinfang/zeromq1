INCLUDES = -I$(top_builddir) -I$(top_srcdir)  -I$(top_builddir)/libzmq \
-I$(top_srcdir)/libzmq

if BUILD_CPU_ESTIMATOR

noinst_PROGRAMS = estimate_cpu_freq

estimate_cpu_freq_SOURCES = estimate_cpu_freq.cpp
estimate_cpu_freq_CXXFLAGS = -Wall -pedantic -Werror
endif
