%{!?py_ver: %define py_ver %(python -c "import sys;print(sys.version[0:3])")}
%{!?py_sitedir: %define py_sitedir %(python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(0,0,'%{_prefix}')")}
%{!?py_sitearch: %define py_sitearch %(python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(1,0,'%{_prefix}')")}

%define java_target 1.5

Name: zeromq
Summary: ZeroMQ is a thin messaging implementation
Version: 0.6
Release: 1
Group: Development/Libraries
License: GPL/LGPL
Source0: %{name}-svn-current.tar.bz2
Source1: %{name}.Key.snk.bz2
URL: http://www.zeromq.org
BuildRequires: gcc-c++
BuildRequires: pkgconfig
BuildRequires: python python-devel
BuildRequires: jpackage-utils
%if "%{_vendor}" == "suse"
BuildRequires:  java-devel >= 1.5 update-alternatives
%if %{sles_version} != 0 && %{sles_version} <= 10
%ifarch "%{ix86}"
BuildRequires:  java-1_4_2-sun-alsa
%endif
%endif
%else
%if "%{_vendor}" == "redhat" && 0%{?fedora} >= 9
BuildRequires:  java-sdk-openjdk >= 1.5.0
%else
BuildRequires:  java-devel >= 1.4
%define java_target 1.4
%endif
%endif
%if "%{_vendor}" == "mandriva"
BuildRequires:  mono mono-devel
%else
BuildRequires: lksctp-tools-devel
BuildRequires:  mono-core mono-devel
%endif
BuildRoot: %{_tmppath}/%{name}-%{version}-buildroot

%description
ZeroMQ is a thin messaging implementation.
ZeroMQ supports different messaging models.
ZeroMQ is already very fast. We're getting 13.4 microseconds end-to-end
latencies and up to 4,100,000 messages a second today.
ZeroMQ is very thin. Requires just a couple of pages in resident memory.
ZeroMQ is fully open sourced LGPL-licensed software written in C++.
ZeroMQ provides C, C++, Java, Python and .NET language APIs.
ZeroMQ supports different wire-level protocols: TCP, PGM, AMQP, SCTP.
ZeroMQ runs on AIX, FreeBSD, HP-UX, Linux, Mac OS X, OpenBSD, QNX Neutrino,
Solaris and Windows.
ZeroMQ supports i386, x86_64, Sparc, Itanium and ARM microarchitectures.
ZeroMQ is fully distributed: no central servers to crash, millions of WAN
and LAN nodes.
ZeroMQ is an extensible framework: kernel-style drivers for custom hardware, protocols or applications.

%package devel
Summary: Header files, libraries and development documentation for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}
Requires: gcc-c++

%description devel
Development files for ZeroMQ

%package python
Summary: Python support for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description python
This package contains the Python modules that are part of %{name}.

%package java
Summary: Development files for using ZeroMQ with Java
Group: Development/Libraries

%description java
ZeroMQ is a thin messaging implementation.
ZeroMQ supports different messaging models.
ZeroMQ is already very fast. We're getting 13.4 microseconds end-to-end
latencies and up to 4,100,000 messages a second today.
ZeroMQ is very thin. Requires just a couple of pages in resident memory.
ZeroMQ is fully open sourced LGPL-licensed software written in C++.
ZeroMQ provides C, C++, Java, Python and .NET language APIs.
ZeroMQ supports different wire-level protocols: TCP, PGM, AMQP, SCTP.
ZeroMQ runs on AIX, FreeBSD, HP-UX, Linux, Mac OS X, OpenBSD, QNX Neutrino,
Solaris and Windows.
ZeroMQ supports i386, x86_64, Sparc, Itanium and ARM microarchitectures.
ZeroMQ is fully distributed: no central servers to crash, millions of WAN
and LAN nodes.
ZeroMQ is an extensible framework: kernel-style drivers for custom hardware, protocols or applications.
This package contains the libraries for building programs which
use ZeroMQ in Java.

%package mono
Summary: Development files for using ZeroMQ with Mono
Group: Development/Libraries

%description mono
ZeroMQ is a thin messaging implementation.
ZeroMQ supports different messaging models.
ZeroMQ is already very fast. We're getting 13.4 microseconds end-to-end
latencies and up to 4,100,000 messages a second today.
ZeroMQ is very thin. Requires just a couple of pages in resident memory.
ZeroMQ is fully open sourced LGPL-licensed software written in C++.
ZeroMQ provides C, C++, Java, Python and .NET language APIs.
ZeroMQ supports different wire-level protocols: TCP, PGM, AMQP, SCTP.
ZeroMQ runs on AIX, FreeBSD, HP-UX, Linux, Mac OS X, OpenBSD, QNX Neutrino,
Solaris and Windows.
ZeroMQ supports i386, x86_64, Sparc, Itanium and ARM microarchitectures.
ZeroMQ is fully distributed: no central servers to crash, millions of WAN
and LAN nodes.
ZeroMQ is an extensible framework: kernel-style drivers for custom hardware, protocols or applications.
This package contains the libraries for building programs which
use ZeroMQ in Mono.

%prep

%setup -n %{name}
[ ! -e %SOURCE1 ] || bunzip2 -c < %SOURCE1 > mono/clrzmq/clrzmq/zmq_strong_name.snk

%build
[ -n "$JAVA_HOME" ] || export JAVA_HOME=%{java_home}
%configure --with-c --with-python --with-java --with-sctp --with-amqp --with-clr --with-clrdir=%{_prefix}/lib/clrzmq
%{__make} %{?jobs:-j%jobs} JAVACFLAGS="-target %{java_target}"
(cd libjzmq && %{jar} cvf libjzmq.jar *.class)

rm -rf examples/*/.deps/ || :

%install
%makeinstall
mkdir -p %buildroot%{_javadir}
cp libjzmq/libjzmq.jar %buildroot%{_javadir}
mkdir -p %buildroot%{_datadir}/pkgconfig
cp mono/clrzmq/clrzmq/*.pc %buildroot%{_datadir}/pkgconfig/
gacutil -i %buildroot%{_prefix}/lib/clrzmq/libclrzmq.dll -f -root %buildroot%{_prefix}/lib -package clrzmq

[ -e %SOURCE1 ] || bzip2 -c < mono/clrzmq/clrzmq/zmq_strong_name.snk > %SOURCE1

%clean
[ -z %buildroot ] || rm -rf %buildroot

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)
%doc AUTHORS ChangeLog
%doc examples
%{_bindir}/*
%{_libdir}/*.so.*
%attr(644,root,man) %{_mandir}/man1/*
%attr(644,root,man) %{_mandir}/man7/*

%files devel
%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la
%attr(644,root,man) %{_mandir}/man3/*

%files python
%defattr(-,root,root)
%{py_sitearch}/*

%files java
%defattr(-,root,root)
%{_javadir}/*

%files mono
%defattr(-,root,root)
%{_prefix}/lib/clrzmq
%{_prefix}/lib/mono/gac/*
%{_prefix}/lib/mono/clrzmq
%{_datadir}/pkgconfig/*

%changelog
* Wed Feb 11 2009 Dirk O. Siebnich <dok@dok-net.net>
- Packaged for RPM
