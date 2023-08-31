include $(DB_BUILD_TOP)/mi_dep.mk
############################################################ source files.
SRC += ./src/base/common/src/st_common.c
SRC += ./src/Mdb_Integrate.cpp
SRC += ./src/Mdb_Base.cpp
SRC += $(DB_BUILD_TOP)/internal/src/json/cjson.c
SRC += $(DB_BUILD_TOP)/internal/src/json/cjson_utils.c
SRC += $(DB_BUILD_TOP)/internal/src/md5/md5.cpp

ifneq ($(interface_vif), disable)
SRC += ./src/base/common/src/st_vif.c
endif

ifneq ($(interface_vpe), disable)
SRC += ./src/base/common/src/st_vpe.c
endif

ifneq ($(interface_divp), disable)
SRC += ./src/base/common/src/st_divp.c
endif

ifneq ($(interface_rgn), disable)
SRC += ./src/base/common/src/st_rgn.c
SRC += ./src/base/common/src/dot_matrix_font.c
SRC += ./src/Mdb_Region.cpp
endif

ifneq ($(interface_fbdev), disable)
SRC += ./src/base/common/src/st_font.c
endif

ifneq ($(interface_vdec), disable)
SRC += SRC += ./src/Mdb_Vdec.cpp
CODEDEFINE += -DFOR_MDB_BUILD
endif

INTERNAL_MODULES := rtsp
CODEDEFINE += $(foreach n,$(INTERNAL_MODULES),-DINTERNAL_$(shell tr 'a-z' 'A-Z' <<< $(n)))
ifeq ($(filter $(INTERNAL_MODULES), rtsp), rtsp)
SUBDIRS += ./src/base/live555/BasicUsageEnvironment
SUBDIRS += ./src/base/live555/groupsock
SUBDIRS += ./src/base/live555/liveMedia
SUBDIRS += ./src/base/live555/UsageEnvironment
SUBDIRS += ./src/base/live555/mediaServer

INC += ./src/base/live555/BasicUsageEnvironment/include
INC += ./src/base/live555/groupsock/include
INC += ./src/base/live555/liveMedia/include
INC += ./src/base/live555/UsageEnvironment/include
INC += ./src/base/live555/mediaServer/include

CODEDEFINE += -DSOCKLEN_T=socklen_t -DNO_SSTREAM=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DBSD=1 -DLOCALE_NOT_USED -DSO_REUSEPORT -DALLOW_SERVER_PORT_REUSE -DALLOW_RTSP_SERVER_PORT_REUSE
endif

INC     +=$(DB_BUILD_TOP)/internal/inc $(DB_BUILD_TOP)/internal/src/base/common/inc
INC     +=$(DB_BUILD_TOP)/internal/inc $(DB_BUILD_TOP)/internal/src/json/include
INC     +=$(DB_BUILD_TOP)/internal/inc $(DB_BUILD_TOP)/internal/src/md5
INC     += $(PROJ_ROOT)/release/include
INC     += $(PROJ_ROOT)/../sdk/misc/fbdev/fbdev_video
INC     += $(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/drivers/sstar/include

OUTPUT_DIR := $(DB_OUT_PATH)/obj/


############################################################ depnedent header files.
include $(DB_BUILD_TOP)/compile.mk
all: gen_obj
clean: clean_files
