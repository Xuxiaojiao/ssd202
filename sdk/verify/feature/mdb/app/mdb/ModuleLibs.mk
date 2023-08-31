include $(DB_BUILD_TOP)/mi_dep.mk
LIBS     += -Wl,-rpath=./
LIBS     += -L$(DB_OUT_PATH)/lib
LIBS     += -L$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/mi_libs/static
LIBS     += -L$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/ex_libs/static
LIBS     += -l$(DB_LIB_NAME)
ifneq ($(interface_vif), disable)
LIBS     += -lmi_vif
endif
ifneq ($(interface_vpe), disable)
LIBS     += -lmi_vpe
endif
ifneq ($(interface_ldc), disable)
LIBS     += -lmi_ldc
endif
ifneq ($(interface_venc), disable)
LIBS     += -lmi_venc
endif
ifneq ($(interface_divp), disable)
LIBS     += -lmi_divp
endif
ifneq ($(interface_isp), disable)
LIBS     += -lmi_isp -lcus3a -lispalgo
endif
ifneq ($(interface_iqserver), disable)
LIBS     += -lmi_iqserver -lfbc_decode
endif
ifneq ($(interface_sensor), disable)
LIBS     += -lmi_sensor
endif
ifneq ($(interface_rgn), disable)
LIBS     += -lmi_rgn
endif
ifneq ($(interface_common), disable)
LIBS     += -lmi_common
endif
ifneq ($(interface_sys), disable)
LIBS     += -lmi_sys
endif
ifneq ($(interface_vdec), disable)
LIBS     += -lmi_vdec
endif
LIBS     += -lcam_fs_wrapper -lcam_os_wrapper -ldl
