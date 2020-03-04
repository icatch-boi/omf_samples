#####
set(VERSION 1)
set(BINARY omfRtspServer)
set(BINARY_SOURCE_FILES _omfRtspServer.cpp)
set(BINARY_DEPEND_LIBS
		pluginrtsp4

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

file(GLOB FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.*)
set(FILE_LIST ${FILE_LIST} makefile)
install(FILES ${FILE_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/demo/rtspserver/)
