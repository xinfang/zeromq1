%{!?py_ver: %define py_ver %(python -c "import sys;print(sys.version[0:3])")}
%{!?py_sitedir: %define py_sitedir %(python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(0,0,'%{_prefix}')")}
%{!?py_sitearch: %define py_sitearch %(python -c "from distutils.sysconfig import get_python_lib; print get_python_lib(1,0,'%{_prefix}')")}

%if "%{_vendor}" == "redhat" && 0%{?rhelversion} <= 5
%{!?with_java:%define with_java 1}
%{!?with_mono:%define with_mono 0}
%{!?with_ruby:%define with_ruby 0}
%else
%{!?with_java:%define with_java 1}
%{!?with_mono:%define with_mono 1}
%if "%{_vendor}" == "suse" && 0%{?sles_version} != 0 && 0%{?sles_version} <= 10
%{!?with_ruby:%define with_ruby 0}
%else
%{!?with_ruby:%define with_ruby 1}
%else
%endif
%endif

%if 0%{?with_ruby} == 1
%{!?rb_archdir: %define rb_archdir %(ruby -rrbconfig -e 'puts Config::CONFIG["archdir"]')}
%endif

%define java_target 1.5

Name: zeromq
Summary: ZeroMQ is a thin messaging implementation
%define major_version 0
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
%if 0%{?with_ruby} == 1
BuildRequires: ruby ruby-devel
%endif
BuildRequires: jpackage-utils
%if 0%{?with_java} == 1
%if "%{_vendor}" == "suse"
BuildRequires:  java-devel >= 1.5 update-alternatives
%if 0%{?sles_version} != 0 && 0%{?sles_version} <= 10
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
%endif
%if "%{_vendor}" == "mandriva"
%if 0%{?with_mono} == 1
BuildRequires:  mono mono-devel
%endif
%else
BuildRequires: lksctp-tools-devel
%if 0%{?with_mono} == 1
BuildRequires:  mono-core mono-devel
%endif
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

%package -n python-%{name}
Summary: Python support for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description -n python-%{name}
This package contains the Python modules that are part of %{name}.

%if 0%{?with_ruby} == 1
%package -n librbzmq%{major_version}
Summary: Ruby support for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description -n librbzmq%{major_version}
This package contains the Ruby modules that are part of %{name}.
%endif

%if 0%{?with_java} == 1
%package -n libjzmq%{major_version}
Summary: Development files for using ZeroMQ with Java
Group: Development/Libraries

%description -n libjzmq%{major_version}
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
%endif

%if 0%{?with_mono} == 1
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
%endif

%prep

%setup -n %{name}
[ ! -e %SOURCE1 ] || bunzip2 -c < %SOURCE1 > mono/clrzmq/clrzmq/zmq_strong_name.snk

%build
%if 0%{?with_java} == 1
[ -n "$JAVA_HOME" ] || export JAVA_HOME=%{java_home}
%endif
%configure --with-c --with-python \
%if 0%{?with_java} == 1
	--with-java \
%endif
%if 0%{?with_ruby} == 1
	--with-ruby --with-ruby-headersdir=%{rb_archdir} \
%endif
%if 0%{?with_mono} == 1
	--with-clr --with-clrdir=%{_prefix}/lib/clrzmq \
%endif
	--with-sctp --with-amqp

%{?make:%make}%{!?make:%__make %{?jobs:-j%jobs} IMPORT_CPPFLAGS+="$RPM_OPT_FLAGS"} \
%if 0%{?with_java} == 1
	JAVACFLAGS="-target %{java_target}"
%endif

rm -rf examples/*/.deps/ || :

%install
[ -z %{buildroot} ] || rm -rf %{buildroot}
mkdir %{buildroot}
%if "%{_vendor}" == "suse"
%{?makeinstall:%makeinstall}%{!?makeinstall:make DESTDIR=%{buildroot} install}
%else
%{?make:%make}%{!?make:make} install DESTDIR=%{buildroot}
%endif
%if 0%{?with_java} == 1
mkdir -p %buildroot%{_javadir}
cp libjzmq/Zmq.jar %buildroot%{_javadir}
%endif
mkdir -p %buildroot%{_datadir}/pkgconfig
%if 0%{?with_mono} == 1
cp mono/clrzmq/clrzmq/*.pc %buildroot%{_datadir}/pkgconfig/
gacutil -i %buildroot%{_prefix}/lib/clrzmq/libclrzmq.dll -f -root %buildroot%{_prefix}/lib -package clrzmq
%endif

[ -e %SOURCE1 ] || bzip2 -c < mono/clrzmq/clrzmq/zmq_strong_name.snk > %SOURCE1

%clean
[ -z %buildroot ] || rm -rf %buildroot

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%if 0%{?with_ruby} == 1
%post -n librbzmq%{major_version} -p /sbin/ldconfig

%postun -n librbzmq%{major_version} -p /sbin/ldconfig
%endif

%if 0%{?with_java} == 1
%post -n libjzmq%{major_version} -p /sbin/ldconfig

%postun -n libjzmq%{major_version} -p /sbin/ldconfig
%endif

%files
%defattr(-, root, root)
%doc AUTHORS ChangeLog
%{_bindir}/*
%{_libdir}/libzmq.so.*
%{_libdir}/libczmq.so.*
%attr(644,root,man) %{_mandir}/man1/*
%attr(644,root,man) %{_mandir}/man7/*

%files devel
%defattr(-,root,root)
%doc examples
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/*.la
%attr(644,root,man) %{_mandir}/man3/*

%files -n python-%{name}
%defattr(-,root,root)
%{py_sitearch}/*

%if 0%{?with_ruby} == 1
%files -n librbzmq%{major_version}
%defattr(-,root,root)
%{_libdir}/librbzmq.so.*
%endif

%if 0%{?with_java} == 1
%files -n libjzmq%{major_version}
%defattr(-,root,root)
%{_libdir}/libjzmq.so.*
%{_javadir}/*
%endif

%if 0%{?with_mono} == 1
%files mono
%defattr(-,root,root)
%{_prefix}/lib/clrzmq
%{_prefix}/lib/mono/gac/*
%{_prefix}/lib/mono/clrzmq
%{_datadir}/pkgconfig/*
%endif

%changelog
* Wed Feb 11 2009 Dirk O. Siebnich <dok@dok-net.net>
- Packaged for RPM

