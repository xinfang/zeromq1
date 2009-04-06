
EXTRA_DIST =  

# Warning: This is an automatically generated file, do not edit!

if ENABLE_DEBUG
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -warn:3 -optimize- -debug "-define:DEBUG;TRACE" "-keyfile:zmq_strong_name.snk"
ASSEMBLY = bin/Debug/libclrzmq.dll
ASSEMBLY_MDB = $(ASSEMBLY).mdb
COMPILE_TARGET = library
PROJECT_REFERENCES = 
BUILD_DIR = bin/Debug/

LIBCLRZMQ_DLL_MDB_SOURCE=bin/Debug/libclrzmq.dll.mdb
LIBCLRZMQ_DLL_MDB=$(BUILD_DIR)/libclrzmq.dll.mdb

endif

if ENABLE_RELEASE
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -warn:4 -optimize+ "-define:TRACE" "-keyfile:zmq_strong_name.snk"
ASSEMBLY = bin/Release/libclrzmq.dll
ASSEMBLY_MDB = 
COMPILE_TARGET = library
PROJECT_REFERENCES = 
BUILD_DIR = bin/Release/

LIBCLRZMQ_DLL_MDB=

endif

AL=al2
SATELLITE_ASSEMBLY_NAME=$(notdir $(basename $(ASSEMBLY))).resources.dll

PROGRAMFILES = \
	$(LIBCLRZMQ_DLL_MDB)  

LINUX_PKGCONFIG = \
	$(CLRZMQ_PC)  


RESGEN=resgen2
	
all: $(ASSEMBLY) $(PROGRAMFILES) $(LINUX_PKGCONFIG) 

FILES = \
	../../../libclrzmq/zmq.cs \
	Properties/AssemblyInfo.cs 

DATA_FILES = 

RESOURCES = 

EXTRAS = \
	zmq_strong_name.snk \
	clrzmq.pc.in 

REFERENCES =  \
	System \
	System.Data \
	System.Xml

DLL_REFERENCES = 

CLEANFILES = $(PROGRAMFILES) $(LINUX_PKGCONFIG) 

include $(top_srcdir)/Makefile.include

CLRZMQ_PC = $(BUILD_DIR)/clrzmq.pc

$(eval $(call emit-deploy-wrapper,CLRZMQ_PC,clrzmq.pc))


$(eval $(call emit_resgen_targets))
$(build_xamlg_list): %.xaml.g.cs: %.xaml
	xamlg '$<'

$(ASSEMBLY) $(ASSEMBLY_MDB): $(build_sources) $(build_resources) $(build_datafiles) $(DLL_REFERENCES) $(PROJECT_REFERENCES) $(build_xamlg_list) $(build_satellite_assembly_list)
	mkdir -p $(shell dirname $(ASSEMBLY))
	$(ASSEMBLY_COMPILER_COMMAND) $(ASSEMBLY_COMPILER_FLAGS) -out:$(ASSEMBLY) -target:$(COMPILE_TARGET) $(build_sources_embed) $(build_resources_embed) $(build_references_ref)
