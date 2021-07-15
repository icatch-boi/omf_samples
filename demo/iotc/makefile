ALL =

SRCS =
SRCS += ipc_shm/ipc_shm.c
SRCS += iotc_console.cpp
SRCS += iotc_io.cpp
SRCS += iotc_listen.cpp
SRCS += iotc_queue.cpp
SRCS += iotc_receiver.cpp
SRCS += iotc_transmitter.cpp

IOTC_KVS ?= YES
ifeq ($(IOTC_KVS), YES)
    ALL+=omfIoTCDemoKvs_
    SRCS += iotc_demo_kvs.cpp
else
    ALL+=omfIoTCDemoPure_
    SRCS += iotc_demo_pure.cpp
endif

Targets?=$(ALL)
###
all:preprc $(Targets)
###
GCC7.3 ?= 1
ifeq ($(GCC7.3), 1)
    PREFIX_= /opt/openwrt_toolchain/arm-openwrt-icatchtek-bsp4-glibc/bin/arm-openwrt-linux-gnueabi-
else
    PREFIX_= /opt/openwrt_toolchain/arm-openwrt-icatchtek-glibc/bin/arm-openwrt-linux-
endif

DST_PATH = ../../lib/

PREFIX?=$(PREFIX_)
LIBS_PATH ?= ../../lib
INCS_PATH ?= ../../include/


OUT_PATH ?= ./out
###
GCC ?= ${PREFIX}gcc
GXX ?= ${PREFIX}g++
LD ?= ${PREFIX}ld
GSTRIP ?= ${PREFIX}strip
###
CFLAGS += -mcpu=cortex-a7 -mabi=aapcs -mfloat-abi=hard -mlittle-endian -mno-unaligned-access -pthread -rdynamic -fPIC -flax-vector-conversions -mfpu=neon-vfpv4 -Os -Wno-error=pointer-sign -Wno-error=char-subscripts -Wno-error=unused-but-set-variable -Wno-error=unused-function -funwind-tables -ffunction-sections
CXXFLAGS += ${CFLAGS} -std=c++11 -fpermissive -fexceptions -Woverloaded-virtual -Wreturn-type -pthread
LDFLAGS += -Wl,--no-as-needed,--copy-dt-needed-entries

###
INCS += -I.
INCS += -I${INCS_PATH}/
INCS += -I${INCS_PATH}/iotc
INCS += -I${INCS_PATH}/iotc/ipc_shm
INCS += -I${INCS_PATH}/misc
INCS += -I${INCS_PATH}/jansson
##
LIBS += -ljansson -lpluginshmbase -lfwbase -lomfapi -lfwporting -lnlmci
###

$(Targets):$(SRCS)
	@echo $@
	$(GXX) $(INCS) $(CXXFLAGS) $^ $(LDFLAGS) -L${LIBS_PATH} $($@LIBS) $(LIBS) -o $@

###	
clean:
	@echo "Removing linked and compiled files......"
	rm -rf $(Targets) *.o ./out

###
install:all
	@mkdir -p $(OUT_PATH)
	@for tmp in $(Targets); \
	do \
		mv $$tmp $(OUT_PATH); \
	done

.PHONY : clean install printGccVersion printAll preprc

preprc: printGccVersion

printAll:
	@echo $(ALL)

gccVer := `$(GCC) -dumpversion`
printGccVersion:
	@echo $(gccVer)
	