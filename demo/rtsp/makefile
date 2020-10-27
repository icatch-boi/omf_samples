ALL =
ALL+=libpluginrtsp4.so
###
CFLAGS +=
CXXFLAGS += -DSOCKLEN_T=socklen_t
CXXFLAGS += -DNO_SSTREAM=1
CXXFLAGS += -D_LARGEFILE_SOURCE=1
CXXFLAGS += -D_FILE_OFFSET_BITS=64
CXXFLAGS += -DLOCALE_NOT_USED
CXXFLAGS += -DREAD_FROM_FILES_SYNCHRONOUSLY
###
INCS += -I${INCS_PATH}/3th/BasicUsageEnvironment/include
INCS += -I${INCS_PATH}/3th/groupsock/include
INCS += -I${INCS_PATH}/3th/UsageEnvironment/include
INCS += -I${INCS_PATH}/3th/liveMedia/include
###
LIBS += -lliveMedia
LIBS += -lgroupsock
LIBS += -lUsageEnvironment
LIBS += -lBasicUsageEnvironment
###
LIBS += -lpluginpcm
LIBS += -lpluginav
###
LIBS += -lomfapi
LIBS += -lomfapicxx
LIBS += -lomfapistreaming
LIBS += -lpluginapistreaming
###
LIBS += -lpluginshmbase
LIBS += -lpluginshm
###
LIBS += -lpluginbase
LIBS += -lfwcore
LIBS += -lfwbase
LIBS += -lfwporting
###
LIBS += -ldl
LIBS += -ldbglinux
###
OBJS +=RtspService.o
OBJS +=RtspServer.o
OBJS +=RtspSession.o
OBJS +=OmfRtspService.o
OBJS +=RtspSmss.o
OBJS +=RtspSmssH264.o
#OBJS +=RtspSmssH265.o
#OBJS +=RtspSmssJpeg.o
#OBJS +=RtspSmssPcm.o
OBJS +=RtspSmssAac.o
OBJS +=RtpSource.o
OBJS +=RtpSink.o
###
include ../makefile.tmpl

