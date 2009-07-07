libclrzmq.dlldir=@CLRDIR@
libclrzmq.dll_DATA = bin/Release/libclrzmq.dll

pkgconfigdir = $(datadir)/pkgconfig
pkgconfig_DATA = libclrzmq.pc

gmcs_options = -noconfig -codepage:utf8 -warn:4 -optimize+ "-define:TRACE"
gmcs_keyfile = "-keyfile:zmq_strong_name.snk"
gmcs_out = -out:bin/Release/libclrzmq.dll
gmcs_assembly_info = ./Properties/AssemblyInfo.cs
gmcs_resources = -r:System -r:System.Data -r:System.Xml

bin/Release/libclrzmq.dll: ../../../libclrzmq/zmq.cs
	$(mkdir_p) bin/Release
	$(GMCS) $(gmcs_options) $(gmcs_keyfile) $(gmcs_out)  -target:library '$^'  '$(gmcs_assembly_info)' $(gmcs_resources)

clean-local:
	-rm -rf bin
