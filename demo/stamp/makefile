ALL =
ALL+=omfStampService_
ALL+=omfStampSet_


###
all:$(ALL)
###
TOOLCHAIN_PATH ?= /home/android_camera/iCatchtek/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin/
#TOOLCHAIN_PATH ?= ../../../bsp3_glibc/staging_dir/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin
#TOOLCHAIN_PATH ?= $(shell pwd)/../../../bsp3_glibc/staging_dir/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin
COMPILER_PREFIX ?= arm-openwrt-linux-gnueabi-
ROOT_PATH ?= ../../../_dualos
LIBS_PATH ?= ${ROOT_PATH}/lib
INCS_PATH ?= ${ROOT_PATH}/include
OUT_PATH ?= ./out
###
GCC ?= ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}gcc
GXX ?= ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}g++
GSTRIP ?= ${TOOLCHAIN_PATH}/${COMPILER_PREFIX}strip
###
CFLAGS += -Wno-error=pointer-sign -Wno-error=char-subscripts -Wno-error=unused-but-set-variable -Wno-error=unused-function
CXXFLAGS += ${CFLAGS} -std=c++11 -Wall -g -O2 -fpermissive -fexceptions -Woverloaded-virtual -Wreturn-type
###



INCS += -I.
INCS += -I${INCS_PATH}/
INCS += -I${INCS_PATH}
INCS += -I${INCS_PATH}/c++
LIBS =
LIBS += -L${LIBS_PATH}
###
LIBS += -lpluginiofile
LIBS += -lpluginwav
LIBS += -lpluginmp4
LIBS += -lpluginav
LIBS += -lpluginpcm
###
LIBS += -lomfapi
LIBS += -lomfapicxx
LIBS += -lomfapistreaming
LIBS += -lpluginapistreaming
###
LIBS += -lshm
LIBS += -lpluginshm
LIBS += -lipc
LIBS += -lpluginipc
###
LIBS += -lpluginbase
LIBS += -lfwcore
LIBS += -lfwbase
LIBS += -lfwporting
###
LIBS += -ldl
LIBS += -ldbglinux
###
%.o:%.cpp
	@echo "compile cxx files..."
	$(GXX) $(INCS) $(CXXFLAGS) -c $^ -o $@

%.o:%.cc
	@echo "compile cxx files..."
	$(GXX) $(INCS) $(CXXFLAGS) -c $^ -o $@

%.o:%.c
	@echo "compile c files...."
	$(GCC) $(INCS) $(CFLAGS) -c $^ -o $@

%_:_%.o
	@echo "link..."
	$(GXX) $(INCS) $(CXXFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

###	
clean:
	@echo "Removing linked and compiled files......"
	rm -rf $(ALL) *.o

###
install:${ALL}
	@mkdir -p $(OUT_PATH)
	@mkdir -p $(OUT_PATH)/bin
	@for tmp in $(ALL); \
	do \
		mv $$tmp $(OUT_PATH)/bin/; \
	done
	