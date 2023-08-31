# ----------------------------------------------------------------------
#
# Project: C&M Video multi instance sample
#
# ----------------------------------------------------------------------
.PHONY: CREATE_DIR LIBTHEORA clean all

PRODUCT := WAVE511

$(shell cp sample_v2/component_list_decoder.h sample_v2/component/component_list.h)

USE_FFMPEG  = yes
USE_PTHREAD = yes
USE_RTL_SIM = no

UNAME = $(shell uname -a)
ifneq (,$(findstring i386, $(UNAME)))
    USE_32BIT = yes
endif

ifeq ($(RTL_SIM), 1)
USE_RTL_SIM = yes
endif

ifeq ($(SINGLE_THREAD), 1)
    USE_SINGLE_THREAD=yes
endif


ifeq ($(PRODUCT), CODA960)
    USE_LIBTHEORA=yes
endif
ifeq ($(PRODUCT), CODA980)
    USE_LIBTHEORA=yes
endif

ifeq ($(USE_32BIT), yes)
PLATFORM    = nativelinux
else
PLATFORM    = nativelinux_64bit
endif

CROSS_CC_PREFIX = arm-linux-gnueabihf-
VDI_C           = vdi/linux/vdi.c
VDI_OSAL_C      = vdi/linux/vdi_osal.c
MM_C            = 
PLATFORM_FLAGS  = 

VDI_VPATH       = vdi/linux
ifeq ("$(BUILD_CONFIGURATION)", "NonOS")
    CROSS_CC_PREFIX = arm-none-eabi-
    VDI_C           = vdi/nonos/vdi.c
    VDI_OSAL_C      = vdi/nonos/vdi_osal.c
    MM_C            = vdi/mm.c
    USE_FFMPEG      = no
    USE_PTHREAD     = no
    PLATFORM        = none
    DEFINES         = -DLIB_C_STUB
    PLATFORM_FLAGS  = 
    VDI_VPATH       = vdi/nonos
endif
ifeq ("$(BUILD_CONFIGURATION)", "EmbeddedLinux")
    CROSS_CC_PREFIX = arm-none-linux-gnueabi-
    PLATFORM        = armlinux
endif

CC  = $(CROSS_CC_PREFIX)gcc
CXX = $(CROSS_CC_PREFIX)g++
LINKER=$(CC)
AR  = $(CROSS_CC_PREFIX)ar

INCLUDES = -I./theoraparser/include -I./vpuapi -I./ffmpeg/include -I./sample_v2/helper -I./sample_v2/component -I./sample_v2/helper/misc -I./vdi
INCLUDES += -I./sample_v2/component_decoder -I./sample_v2/component_encoder
DEFINES += -D$(PRODUCT)
INCLUDES += -I./../zlib/zlib-1.2.2/host/include
ifeq ($(USE_RTL_SIM), yes)
    DEFINES += -DCNM_SIM_PLATFORM -DCNM_SIM_DPI_INTERFACE -DSUPPORT_MULTI_INSTANCE_TEST -DUSE_SINGLE_THREAD
    USE_SINGLE_THREAD=yes
    MM_C            = vdi/mm.c
else	# USE_RTL_SIM
endif	# USE_RTL_SIM

ifeq ($(USE_SINGLE_THREAD), yes)
    DEFINES += -DUSE_SINGLE_THREAD
endif

CFLAGS  += -g -I. -Wno-implicit-function-declaration -Wno-format -Wl,--fatal-warning $(INCLUDES) $(DEFINES) $(PLATFORM_FLAGS)
ifeq ($(USE_RTL_SIM), yes)
ifeq ($(IUS), 1)
    CFLAGS  += -fPIC # ncverilog is 64bit version
endif
endif
ARFLAGS += cru

ifeq ($(USE_LIBTHEORA), yes)
    LDFLAGS  += $(PLATFORM_FLAGS) -L./theoraparser/
    LDLIBS   += -ltheoraparser
endif

ifeq ($(USE_FFMPEG), yes)
    LDLIBS  += -lavformat -lavcodec -lavutil
#LDFLAGS += -L./ffmpeg/lib/$(PLATFORM)
LDFLAGS += -L./ffmpeg/host/lib
LDFLAGS += -L./zlib/zlib-1.2.2/host/lib
    ifneq ($(USE_32BIT), yes)
        LDLIBS  += -lz
    endif #USE_32BIT
endif #USE_FFMPEG

ifeq ($(USE_PTHREAD), yes)
    LDLIBS  += -lpthread
endif
LDLIBS  += -lm

TARGET=multi_instance_test
MAKEFILE=Wave5xxMultiV2.mak
OBJDIR=obj
ALLOBJS=*.o
ALLDEPS=*.dep
ALLLIBS=*.a
RM=rm -f
MKDIR=mkdir -p

SOURCES = main_multi_instance_test.c        \
          component_dec_decoder.c           \
          component_dec_feeder.c            \
          component_dec_renderer.c          \
          decoder_listener.c                \
          cnm_app.c                         \
          cnm_task.c                        \
          component.c                       \
          main_helper.c                     \
          vpuhelper.c                       \
          bitstreamfeeder.c                 \
          bitstreamreader.c                 \
          bsfeeder_fixedsize_impl.c         \
          bsfeeder_framesize_impl.c         \
          bsfeeder_size_plus_es_impl.c      \
          comparator.c                      \
          md5_comparator_impl.c             \
          yuv_comparator_impl.c             \
          cnm_video_helper.c                \
          container.c                       \
          datastructure.c                   \
          debug.c                           \
          bw_monitor.c                      \
          pf_monitor.c      
SOURCES += $(VDI_C)                                         \
          $(VDI_OSAL_C)                                     \
          $(MM_C)                                           \
          vpuapi/product.c                                  \
          vpuapi/vpuapifunc.c                               \
          vpuapi/vpuapi.c                                   \
          vpuapi/coda9/coda9.c                              \
          vpuapi/wave/wave5.c

ifeq ($(USE_RTL_SIM), yes)
    SOURCES += main_sim.c
endif

VPATH  = sample_v2:
VPATH += sample_v2/component_encoder:
VPATH += sample_v2/component_decoder:
VPATH += sample_v2/helper:
VPATH += sample_v2/helper/bitstream:
VPATH += sample_v2/helper/comparator:
VPATH += sample_v2/helper/display:sample_v2/helper/misc:sample_v2/helper/yuv:sample_v2/component:
VPATH += vdi:
ifeq ($(HAPS), 1)
    VPATH += vdi/haps
endif
ifeq ($(HAPS), 2)
    VPATH += vdi/haps
endif
VPATH += $(VDI_VPATH):vpuapi:vpuapi/coda9:vpuapi/wave

OBJECTNAMES=$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))
OBJECTPATHS=$(addprefix $(OBJDIR)/,$(notdir $(OBJECTNAMES)))

target_list = CREATE_DIR $(OBJECTPATHS)
ifeq ($(USE_LIBTHEORA), yes)
    target_list += LIBTHEORA
endif

ifeq ($(USE_RTL_SIM), yes)
all: $(target_list)
else
all: $(target_list)
	$(LINKER) -o $(TARGET) $(LDFLAGS) -Wl,-gc-section -Wl,--start-group $(OBJECTPATHS) $(LDLIBS) -Wl,--end-group
endif

-include $(OBJECTPATHS:.o=.dep)

clean: 
	$(RM) $(TARGET)
	$(RM) $(OBJDIR)/$(ALLOBJS)
	$(RM) $(OBJDIR)/$(ALLDEPS)
	$(RM) theoraparser/$(ALLOBJS)
	$(RM) theoraparser/$(ALLLIBS)
	$(RM) theoraparser/$(ALLDEPS)

LIBTHEORA: 
	cd theoraparser; make clean; make

CREATE_DIR:
	-mkdir -p $(OBJDIR)

obj/%.o: %.c $(MAKEFILE)
	$(CC) $(CFLAGS) -Wall -Werror -c $< -o $@ -MD -MF $(@:.o=.dep)

