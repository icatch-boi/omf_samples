ALL =

SRCS =
SRCS += inifile.c
SRCS += remote_push_io.c
SRCS += remote_push_queue.c
SRCS += remote_push_interface.c
SRCS += remote_push_demo.c
			
ALL+=omfDemoRemotePush_


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
INCS += -I${INCS_PATH}/remotepush
INCS += -I${INCS_PATH}/misc
##
LIBS += -lpluginshmbase -lfwbase -lfwporting -lomfapi -ljansson -lmbedcrypto -lmbedtls -lmbedx509 -ltinyaiot_device -ltinyaiot_common -lpthread

###

$(Targets):$(SRCS)
	@echo $@
	$(GCC) $(INCS) $(CFLAGS) $^ $(LDFLAGS) -L${LIBS_PATH} $($@LIBS) $(LIBS) -o $@

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
	