
EXTRA_DIST =  

# Warning: This is an automatically generated file, do not edit!

pkglibdir = @CLRDIR@

if ENABLE_DEBUG
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -warn:3 -optimize- "-define:DEBUG;TRACE" "-keyfile:zmq_strong_name.snk"
ASSEMBLY = bin/Debug/libclrzmq.dll
ASSEMBLY_MDB = 
COMPILE_TARGET = library
PROJECT_REFERENCES = 
BUILD_DIR = bin/Debug/


endif

if ENABLE_RELEASE
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -warn:4 -optimize+ "-define:TRACE" "-keyfile:zmq_strong_name.snk"
ASSEMBLY = bin/Release/libclrzmq.dll
ASSEMBLY_MDB = 
COMPILE_TARGET = library
PROJECT_REFERENCES = 
BUILD_DIR = bin/Release/


endif

AL=al2
SATELLITE_ASSEMBLY_NAME=$(notdir $(basename $(ASSEMBLY))).resources.dll


RESGEN=resgen2
	
all: $(ASSEMBLY) 

FILES = \
	../../../libclrzmq/zmq.cs \
	Properties/AssemblyInfo.cs 

DATA_FILES = 

RESOURCES = 

EXTRAS = \
	zmq_strong_name.snk 

REFERENCES =  \
	System \
	System.Data \
	System.Xml

DLL_REFERENCES = 

CLEANFILES = 

include $(top_srcdir)/mono/Makefile.include




$(eval $(call emit_resgen_targets))
$(build_xamlg_list): %.xaml.g.cs: %.xaml
	xamlg '$<'

$(ASSEMBLY) $(ASSEMBLY_MDB): $(build_sources) $(build_resources) $(build_datafiles) $(DLL_REFERENCES) $(PROJECT_REFERENCES) $(build_xamlg_list) $(build_satellite_assembly_list)
	mkdir -p $(shell dirname $(ASSEMBLY))
	$(ASSEMBLY_COMPILER_COMMAND) $(ASSEMBLY_COMPILER_FLAGS) -out:$(ASSEMBLY) -target:$(COMPILE_TARGET) $(build_sources_embed) $(build_resources_embed) $(build_references_ref)


install-data-hook:
	-rmdir $(libdir)/zmq
