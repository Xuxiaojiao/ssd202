ifeq ($(CHIP),i2)
include clear-config.mk
COMPILE_TARGET:=bin
WITH_DISP_TEST?=yes
SRCS:= vpe_yuv420_in_disp_venc_tc002.c ../vpe_test_common.c
ifeq ($(WITH_DISP_TEST),yes)
CFLAGS += -DENABLE_WITH_DISP_TEST
LIBS:= mi_sys mi_vpe mi_disp mi_hdmi mi_venc
else
LIBS:= mi_sys mi_vpe
endif
include add-config.mk
endif
