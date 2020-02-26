ALL =
ALL+=demoRtspServer_

###
all:$(ALL)
###
TOOLCHAIN_PATH ?= /home/android_camera/iCatchtek/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin/
#TOOLCHAIN_PATH ?= ../../bsp3_glibc/staging_dir/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin
#TOOLCHAIN_PATH ?= $(shell pwd)/../../../../../bsp3_glibc/staging_dir/toolchain-arm_cortex-a7+vfp_gcc-4.8-linaro_glibc-2.19_eabi/bin
COMPILER_PREFIX ?= arm-openwrt-linux-gnueabi-
ROOT_PATH ?= ../../../_amaryllo
LIBS_PATH ?= ${ROOT_PATH}/lib
INCS_PATH ?= ${ROOT_PATH}/include
LIVE555_PATH ?= ../../live555/live555
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
INCS += -I${INCS_PATH}/streaming
INCS += -I${INCS_PATH}/streaming/common
INCS += -I${LIVE555_PATH}
INCS += -I${LIVE555_PATH}/BasicUsageEnvironment/include
INCS += -I${LIVE555_PATH}/groupsock/include
INCS += -I${LIVE555_PATH}/UsageEnvironment/include
INCS += -I${LIVE555_PATH}/liveMedia/include
###
LIBS += -L${LIBS_PATH}
LIBS += -lliveMedia
LIBS += -lgroupsock
LIBS += -lUsageEnvironment
LIBS += -lBasicUsageEnvironment
###
LIBS += -lomfapi
LIBS += -lomfapicxx
LIBS += -lomfapistreaming
LIBS += -lpluginapistreaming
###
LIBS += -lpluginav
LIBS += -lpluginbase
###
LIBS += -lshm
LIBS += -lpluginshm
LIBS += -lipc
LIBS += -lpluginipc
###
LIBS += -lfwcore
LIBS += -lfwbase
LIBS += -lfwporting
###
LIBS += -ldl
LIBS += -ldbglinux

OBJS +=RtspService.o
OBJS +=RtspServer.o
OBJS +=RtspSession.o
OBJS +=IRtspService.o
OBJS +=RtspSmss.o
OBJS +=RtspSmssH264.o
#OBJS +=RtspSmssH265.o
#OBJS +=RtspSmssJpeg.o
#OBJS +=RtspSmssPcm.o
OBJS +=RtspSmssAac.o
OBJS +=RtpSource.o
OBJS +=RtpSink.o

###
%.o:%.cpp
	@echo "compile cxx files..."
	$(GXX) $(INCS) $(CXXFLAGS) -c $^ -o $@

%.o:%.c
	@echo "compile c files...."
	$(GCC) $(INCS) $(CFLAGS) -c $^ -o $@

%_:_%.o ${OBJS}
	@echo "$@ link..."
	@echo  $($@LIBS)...
	$(GXX) $(CXXFLAGS) $^ $(LIBS) $($@LIBS) $(LDFLAGS) -o $@

###
clean:
	@echo "Removing linked and compiled files......"
	rm -rf $(ALL) *.o

###
install:${ALL}
	@mkdir -p $(OUT_PATH)
	@mkdir -p $(OUT_PATH)/bin
	for tmp in $(ALL); \
	do \
		cp -rf $$tmp $(OUT_PATH)/bin/; \
	done
