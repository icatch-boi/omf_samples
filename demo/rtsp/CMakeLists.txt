set(LIBRARY pluginrtsp4)
set(VERSION 2)

set(LIBRARY_SOURCE_FILES
		OmfRtspService.cpp
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

#set(LIBRARY_ADDITION_DEFINITION
#		-DSOCKLEN_T=socklen_t
#		-DNO_SSTREAM=1
#		-D_LARGEFILE_SOURCE=1
#		-D_FILE_OFFSET_BITS=64
#		-DLOCALE_NOT_USED
#		-DREAD_FROM_FILES_SYNCHRONOUSLY
#		#-DDEBUG=1
#		)
set(LIBRARY_DEPEND_LIBS
		omfapi
		omfapicxx
		omfapistreaming

		liveMedia
		groupsock
		UsageEnvironment
		BasicUsageEnvironment
		)
regLibrary()
add_dependencies(${LIBRARY}
		3th_live555
		)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(demo)

###release
file(GLOB FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/Omf*.h)
install(FILES ${FILE_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX_INC}/misc)
###release demo
file(GLOB FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
set(FILE_LIST ${FILE_LIST} makefile)
install(FILES ${FILE_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/demo/rtsp)
