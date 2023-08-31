.PHONY :all clean gen_exe gen_obj clean_files gen_lib

include $(PROJ_ROOT)/configs/current.configs
include $(DB_BUILD_TOP)/define.mk
ifneq ($(USE_X86), 1)
include $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/$(BOARD)/$(TOOLCHAIN)/toolchain.mk
else
CC := gcc
CXX := g++
AR := ar
STRIP := strip
GCCFLAGS := -Wall -g
endif

#GCCFLAGS := -Wall -g -Werror
GCCFLAGS ?= -Wall -g -mthumb -pipe -fPIC
CXXFLAGS := $(GCCFLAGS) $(LOCAL_CXXFLAGS)
CXXFLAGS += $(CODEDEFINE) -DLINUX_OS -std=c++11
CXXFLAGS += $(foreach dir,$(INC),-I$(dir))

CFLAGS := $(GCCFLAGS) $(LOCAL_CFLAGS)
CFLAGS += $(CODEDEFINE) -DLINUX_OS
CFLAGS += $(foreach dir,$(INC),-I$(dir))

SRC    +=  $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.c)) $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.cpp))
ifneq ($(USE_X86), 1)
OBJS_CXX := $(patsubst %.cpp,%.o,$(filter %.cpp, $(SRC)))
OBJS     := $(patsubst %.c,%.o,$(filter %.c, $(SRC)))
else
OBJS_CXX := $(patsubst %.cpp,%_x86.o,$(filter %.cpp, $(SRC)))
OBJS     := $(patsubst %.c,%_x86.o,$(filter %.c, $(SRC)))
endif

DFILES := $(foreach dir,$(SUBDIRS),$(wildcard $(dir)/*.d))
sinclude $(DFILES)

ifneq ($(USE_X86), 1)
$(OBJS):%.o:%.c
	@echo compile $<...
	@$(CC) $(CFLAGS) -MM $< | sed -e '1s/$(notdir $@):/$(subst /,\/,$@): $(subst /,\/,$(@:.o=.d))/' > $(@:.o=.d)
	@$(CC) $(CFLAGS) -c -ffunction-sections -fdata-sections $< -o $@
$(OBJS_CXX):%.o:%.cpp
	@echo compile $<...
	@$(CXX) $(CXXFLAGS) -MM $< | sed -e '1s/$(notdir $@):/$(subst /,\/,$@): $(subst /,\/,$(@:.o=.d))/' > $(@:.o=.d)
	@$(CXX) $(CXXFLAGS) -c -ffunction-sections -fdata-sections $< -o $@
else
$(OBJS):%_x86.o:%.c
	@echo compile $<...
	@$(CC) $(CFLAGS) -MM $< | sed -e '1s/$(notdir $@):/$(subst /,\/,$@): $(subst /,\/,$(@:.o=.d))/' > $(@:.o=.d)
	@$(CC) $(CFLAGS) -c $< -o $@
$(OBJS_CXX):%_x86.o:%.cpp
	@echo compile $<...
	@$(CXX) $(CXXFLAGS) -MM $< | sed -e '1s/$(notdir $@):/$(subst /,\/,$@): $(subst /,\/,$(@:.o=.d))/' > $(@:.o=.d)
	@$(CXX) $(CXXFLAGS) -c $< -o $@
endif

gen_exe:$(OBJS_CXX) $(OBJS)
ifneq ($(OBJS_CXX), )
	@mkdir -p $(OUTPUT_DIR)
	@$(CXX) $(CXXFLAGS) -Wl,--gc-sections $(OBJS_CXX) $(OBJS) $(LIBS) -o $(OUTPUT_DIR)/$(EXEFILE)
endif
ifneq ($(OBJS), )
	@mkdir -p $(OUTPUT_DIR)
	@$(CC) $(CFLAGS) -Wl,--gc-sections $(OBJS) $(LIBS) -o $(OUTPUT_DIR)/$(EXEFILE)
endif

gen_obj:$(OBJS_CXX) $(OBJS)
ifneq ($(OBJS_CXX), )
	@mkdir -p $(OUTPUT_DIR)
	@cp $(OBJS_CXX) $(OUTPUT_DIR) -v
endif
ifneq ($(OBJS), )
	@mkdir -p $(OUTPUT_DIR)
	@cp $(OBJS) $(OUTPUT_DIR) -v
endif

gen_lib:
ifeq ($(LIB_TYPE), static)
	@$(AR) sq lib$(LIB_NAME).a $(OBJ_FILES)
ifneq ($(OUTPUT_DIR), )
	@mkdir -p $(OUTPUT_DIR)
	@mv ./lib$(LIB_NAME).a $(OUTPUT_DIR) -v
endif
endif
ifeq ($(LIB_TYPE), dynamic)
	@$(CC) -shared -fPIC -o lib$(LIB_NAME).so $(OBJ_FILES)
ifneq ($(OUTPUT_DIR), )
	@mkdir -p $(OUTPUT_DIR)
	@mv ./lib$(LIB_NAME).so $(OUTPUT_DIR) -v
endif
endif
ifneq ($(LIB_TYPE), )
	@echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
	@echo lib$(LIB_NAME) is ready!!!!!!!
	@echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
else
	@echo "Error LIB_TYPE not set!!"
endif

clean_files:
	@rm -rvf $(OBJS_CXX) $(OBJS) $(OBJS_CXX:.o=.d) $(OBJS:.o=.d)
	@rm -rvf $(OUTPUT_DIR)
ifneq ($(EXEFILE), )
	@rm -rvf $(OUTPUT_DIR)/$(EXEFILE)
endif
