set(LIBRARY pluginrtsp4)
set(VERSION 1)

set(LIBRARY_SOURCE_FILES
		IRtspService.cpp
		###
		RtspService.cpp
		RtspServer.cpp
		RtspSession.cpp
		###
		RtspSmss.cpp
		RtspSmssH264.cpp
		#RtspSmssH265.cpp
		#RtspSmssJpeg.cpp
		#RtspSmssPcm.cpp
		RtspSmssAac.cpp
		RtpSource.cpp
		RtpSink.cpp
		)

set(LIBRARY_ADDITION_DEFINITION
		-DSOCKLEN_T=socklen_t
		-DNO_SSTREAM=1
		-D_LARGEFILE_SOURCE=1
		-D_FILE_OFFSET_BITS=64
		-DLOCALE_NOT_USED
		-DREAD_FROM_FILES_SYNCHRONOUSLY
		#-DDEBUG=1
		)
set(LIBRARY_DEPEND_LIBS
		omfapi
		omfapicxx
		omfapistreaming
		pluginapistreaming

		liveMedia
		groupsock
		UsageEnvironment
		BasicUsageEnvironment

		pluginbase
		)
regLibrary()

#####
set(VERSION 1)
set(BINARY demoRtspServer)
set(BINARY_SOURCE_FILES _demoRtspServer.cpp)
set(BINARY_DEPEND_LIBS
		pluginrtsp4
		)
if(LINUX)
	set(BINARY_DEPEND_LIBS ${BINARY_DEPEND_LIBS}
			pluginshm
			shm
			pluginipc
			ipc
			dbglinux
			)
	set(BINARY_ADDITION_DEFINITION -D__SHM)
endif()
regBinary()

if(release_rtsp)
	file(GLOB_RECURSE FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*)
	install(FILES ${FILE_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/demo/rtspserver/)
endif()