
EXTRA_DIST =  

# Warning: This is an automatically generated file, do not edit!

if ENABLE_DEBUG
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -unsafe -warn:4 -optimize- -debug "-define:DEBUG;TRACE"
ASSEMBLY = bin/Debug/cs_remote_lat.exe
ASSEMBLY_MDB = $(ASSEMBLY).mdb
COMPILE_TARGET = exe
PROJECT_REFERENCES = 
BUILD_DIR = bin/Debug/

LIBCLRZMQ_DLL_SOURCE=../../../../../../usr/lib/mono/gac/libclrzmq/1.0.0.0__84b2a312945d4e9e/libclrzmq.dll
CS_REMOTE_LAT_EXE_MDB_SOURCE=bin/Debug/cs_remote_lat.exe.mdb
CS_REMOTE_LAT_EXE_MDB=$(BUILD_DIR)/cs_remote_lat.exe.mdb
LIBCLRZMQ_DLL_MDB_SOURCE=../../../../../../usr/lib/mono/gac/libclrzmq/1.0.0.0__84b2a312945d4e9e/libclrzmq.dll.mdb

endif

if ENABLE_RELEASE
ASSEMBLY_COMPILER_COMMAND = gmcs
ASSEMBLY_COMPILER_FLAGS =  -noconfig -codepage:utf8 -unsafe -warn:4 -optimize+ "-define:TRACE"
ASSEMBLY = bin/Release/cs_remote_lat.exe
ASSEMBLY_MDB = 
COMPILE_TARGET = exe
PROJECT_REFERENCES = 
BUILD_DIR = bin/Release/

LIBCLRZMQ_DLL_SOURCE=../../../../../../usr/lib/mono/gac/libclrzmq/1.0.0.0__84b2a312945d4e9e/libclrzmq.dll
CS_REMOTE_LAT_EXE_MDB=
LIBCLRZMQ_DLL_MDB_SOURCE=../../../../../../usr/lib/mono/gac/libclrzmq/1.0.0.0__84b2a312945d4e9e/libclrzmq.dll.mdb

endif

AL=al2
SATELLITE_ASSEMBLY_NAME=$(notdir $(basename $(ASSEMBLY))).resources.dll

PROGRAMFILES = \
	$(LIBCLRZMQ_DLL) \
	$(CS_REMOTE_LAT_EXE_MDB) \
	$(LIBCLRZMQ_DLL_MDB)  

BINARIES = \
	$(CS_REMOTE_LAT)  


RESGEN=resgen2
	
all: $(ASSEMBLY) $(PROGRAMFILES) $(BINARIES) 

FILES = \
	../../../perf/tests/zmq/cs_remote_lat.cs 

DATA_FILES = 

RESOURCES = 

EXTRAS = \
	cs_remote_lat.in 

REFERENCES =  \
	System

DLL_REFERENCES =  \
	../../../../../../usr/lib/mono/gac/libclrzmq/1.0.0.0__84b2a312945d4e9e/libclrzmq.dll

CLEANFILES = $(PROGRAMFILES) $(BINARIES) 

include $(top_srcdir)/Makefile.include

CS_REMOTE_LAT = $(BUILD_DIR)/cs_remote_lat
LIBCLRZMQ_DLL = $(BUILD_DIR)/libclrzmq.dll
LIBCLRZMQ_DLL_MDB = $(BUILD_DIR)/libclrzmq.dll.mdb

$(eval $(call emit-deploy-wrapper,CS_REMOTE_LAT,cs_remote_lat,x))
$(eval $(call emit-deploy-target,LIBCLRZMQ_DLL))
$(eval $(call emit-deploy-target,LIBCLRZMQ_DLL_MDB))


$(eval $(call emit_resgen_targets))
$(build_xamlg_list): %.xaml.g.cs: %.xaml
	xamlg '$<'

$(ASSEMBLY) $(ASSEMBLY_MDB): $(build_sources) $(build_resources) $(build_datafiles) $(DLL_REFERENCES) $(PROJECT_REFERENCES) $(build_xamlg_list) $(build_satellite_assembly_list)
	mkdir -p $(shell dirname $(ASSEMBLY))
	$(ASSEMBLY_COMPILER_COMMAND) $(ASSEMBLY_COMPILER_FLAGS) -out:$(ASSEMBLY) -target:$(COMPILE_TARGET) $(build_sources_embed) $(build_resources_embed) $(build_references_ref)
