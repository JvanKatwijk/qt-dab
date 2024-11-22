
set(devicesLibName ${objectName}_devices)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Xml REQUIRED)

set($(devicesLibName)_HDRS
        ${${devicesLibName}_HDRS}
        device-handler.h
        device-chooser.h
	filereaders/filereader-widget.h
        filereaders/xml-filereader/element-reader.h
        filereaders/xml-filereader/xml-filereader.h
        filereaders/xml-filereader/xml-reader.h
        filereaders/xml-filereader/xml-descriptor.h
        filereaders/rawfiles-new/rawfiles.h
        filereaders/rawfiles-new/raw-reader.h
        filereaders/new-reader/new-reader.h
        filereaders/new-reader/newfiles.h
        filereaders/new-reader/riff-reader.h
)

set(${devicesLibName}_SRCS
        ${${devicesLibName}_SRCS}
        device-handler.cpp
        device-chooser.cpp
        filereaders/xml-filereader/xml-filereader.cpp
        filereaders/xml-filereader/xml-reader.cpp
        filereaders/xml-filereader/xml-descriptor.cpp
        filereaders/rawfiles-new/rawfiles.cpp
        filereaders/rawfiles-new/raw-reader.cpp
        filereaders/new-reader/new-reader.cpp
        filereaders/new-reader/newfiles.cpp
        filereaders/new-reader/riff-reader.cpp
)

include_directories(
	.
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${QT6Widgets_INCLUDES}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT6Widgets_INCLUDES}
        ${QT_QTCORE_INCLUDE_DIR}
	filereaders
	filereaders/xml-filereader
        filereaders/rawfiles-new
        filereaders/wavfiles-new
)

	if (SDRPLAY_V2)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./sdrplay-handler-v2/sdrplay-widget-v2.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./drplay-handler-v2/sdrplay-handler-v2.h
	        ./sdrplay-handler-v2/sdrplayselect.h
	   )

	   include_directories (
	     ./sdrplay-handler-v2
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./sdrplay-handler-v2/sdrplay-handler-v2.h
	        ./sdrplay-handler-v2/sdrplayselect.h
	        ./sdrplay-handler-v2/mirsdrapi-rsp.h
           )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./sdrplay-handler-v2/sdrplay-handler-v2.cpp
	        ./sdrplay-handler-v2/sdrplayselect.cpp
	   )

	   add_definitions (-DHAVE_SDRPLAY_V2)
	endif (SDRPLAY_V2)

	if (SDRPLAY_V3)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./sdrplay-handler-v3/sdrplay-widget-v3.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./sdrplay-handler-v3/sdrplay-handler-v3.h
	        ./sdrplay-handler-v3/sdrplay-commands.h 
	        ./sdrplay-handler-v3/Rsp-device.h 
	        ./sdrplay-handler-v3/RspI-handler.h 
	        ./sdrplay-handler-v3/Rsp1A-handler.h 
	        ./sdrplay-handler-v3/RspII-handler.h 
	        ./sdrplay-handler-v3/RspDuo-handler.h 
	        ./sdrplay-handler-v3/RspDx-handler.h
	   )

	   include_directories (
	     ./sdrplay-handler-v3
	     ./sdrplay-handler-v3/include
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./sdrplay-handler-v3/sdrplay-handler-v3.h
	        ./sdrplay-handler-v3/sdrplay-commands.h
	        ./sdrplay-handler-v3/include/sdrplay_api.h
	        ./sdrplay-handler-v3/Rsp-device.h 
	        ./sdrplay-handler-v3/RspI-handler.h 
	        ./sdrplay-handler-v3/Rsp1A-handler.h 
	        ./sdrplay-handler-v3/RspII-handler.h 
	        ./sdrplay-handler-v3/RspDuo-handler.h 
	        ./sdrplay-handler-v3/RspDx-handler.h
           )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./sdrplay-handler-v3/sdrplay-handler-v3.cpp
                ./sdrplay-handler-v3/Rsp-device.cpp 
	        ./sdrplay-handler-v3/RspI-handler.cpp 
	        ./sdrplay-handler-v3/Rsp1A-handler.cpp 
	        ./sdrplay-handler-v3/RspII-handler.cpp 
	        ./sdrplay-handler-v3/RspDuo-handler.cpp 
	        ./sdrplay-handler-v3/RspDx-handler.cpp 
	   )

	 add_definitions (-DHAVE_SDRPLAY_V3)
	endif (SDRPLAY_V3)

	if (AIRSPY)
#           find_package(LibAIRSPY)
#           if (NOT LIBAIRSPY_FOUND)
#               message(FATAL_ERROR "please install airspy library")
#           endif ()
#	   ## include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./airspy-2/airspy-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./airspy-2/airspy-2.h
	        ./airspy-2/airspyselect.h
	   )

	   include_directories (
	     ./airspy-2
	     ./airspy-2/libairspy
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./airspy-2/airspy-2.h
	        ./airspy-2/airspyselect.h
	        ./airspy-2/libairspy/airspy.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./airspy-2/airspy-2.cpp
	        ./airspy-2/airspyselect.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY_2)
	endif (AIRSPY)

	if (HACKRF)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./hackrf-handler/hackrf-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./hackrf-handler/hackrf-handler.h
	   )

	   include_directories (
	     ./hackrf-handler
	     ./hackrf-handler/libhackrf
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./hackrf-handler/hackrf-handler.h
	        ./hackrf-handler/libhackrf/hackrf.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./hackrf-handler/hackrf-handler.cpp
	   )

	   add_definitions (-DHAVE_HACKRF)
	endif (HACKRF)
#
	if (PLUTO)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./pluto-handler/pluto-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./pluto-handler/pluto-handler.h
	   )

	   include_directories (
	     ./pluto-handler
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./pluto-handler/pluto-handler.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./pluto-handler/pluto-handler.cpp
	   )

	   add_definitions (-DHAVE_PLUTO)
	endif (PLUTO)
#
	if (LIMESDR)
	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./lime-handler/lime-handler.h
	        ./lime-handler/lime-widget.h
	   )

	   include_directories (
	     ./lime-handler
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./lime-handler/lime-handler.h
	        ./lime-handler/lime-widget.h
	        ./lime-handler/LimeSuite.h
	        ./lime-handler/LMS7002M_parameters.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./lime-handler/lime-handler.cpp
	   )

	   add_definitions (-DHAVE_LIME)
	endif (LIMESDR)

	if (RTLSDR_LINUX)
	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ./rtlsdr-handler-linux/rtlsdr-widget.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./rtlsdr-handler-linux/rtl-dongleselect.h
	        ./rtlsdr-handler-linux/rtlsdr-handler.h
	   )

	   include_directories (
	        ./rtlsdr-handler-linux/
	   )

	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ./rtlsdr-handler-linux/rtlsdr-handler.h
	        ./rtlsdr-handler-linux/rtl-dongleselect.h
	        ./rtlsdr-handler-linux/rtl-sdr.h
	        ./rtlsdr-handler-linux/rtl-sdr_export.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./rtlsdr-handler-linux/rtlsdr-handler.cpp
	        ./rtlsdr-handler-linux/rtl-dongleselect.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()

	if (UHD)
	   find_package(UHD REQUIRED)
	   if (NOT UHD_FOUND)
	      message(FATAL_ERROR "please install UHD library")
	   endif ()
	   list(APPEND extraLibs ${UHD_LIBRARIES})

	   set(${devicesLibName}_UIS
	       ${${devicesLibName}_UIS}
               uhd/uhd-widget.ui
	   )

	   set(${devicesLibName}_MOCS
	      ${${devicesLibName}_MOCS}
	      uhd/uhd-handler.h
	   )

	   include_directories(
	      uhd/
	   )

	   set(${devicesLibName}_HDRS
	      ${${devicesLibName}_HDRS}
	      uhd/uhd-handler.h
	   )

	   set(${devicesLibName}_SRCS
	      ${${devicesLibName}_SRCS}
	      uhd/uhd-handler.cpp
	   )

	   add_definitions(-DHAVE_UHD)
	endif ()

	if (SOAPY)
	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ./soapy/soapy-widget.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./soapy/soapy-handler.h
	        ./soapy/soapy-converter.h
	   )

	   include_directories (
	        ./soapy/
	   )

	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ./soapy/soapy-handler.h
	        ./soapy/soapy-converter.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./soapy/soapy-handler.cpp
	        ./soapy/soapy-converter.cpp
	   )

	   set (LIBSOAPY "-lSoapy -lm")
	   list(APPEND extraLibs ${LIBSOAPY})
	   add_definitions (-DHAVE_SOAPY)
	endif()

	if (SPY_SERVER_8)
	   include_directories (
	        ./spy-server-8/
	   )

	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ./spy-server-8/spyserver-widget-8.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./spy-server-8/spyserver-client-8.h
	        ./spy-server-8/spy-handler-8.h
	        ./spy-server-8/tcp-client-8.h
	   )


	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ./spy-server-8/spyserver-client-8.h
	        ./spy-server-8/spy-handler-8.h
	        ./spy-server-8/tcp-client-8.h
	        ./spy-server-8/spyserver-protocol.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./spy-server-8/spyserver-client-8.cpp
	        ./spy-server-8/spy-handler-8.cpp
	        ./spy-server-8/tcp-client-8.cpp
	   )

	   add_definitions (-DHAVE_SPYSERVER_8)
	endif()

	if (SPY_SERVER_16)
	   include_directories (
	        ./spy-server-16/
	   )

	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ./spy-server-16/spyserver-widget-16.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./spy-server-16/spyserver-client-16.h
	        ./spy-server-16/spy-handler-16.h
	        ./spy-server-16/tcp-client-16.h
	   )


	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ./spy-server-16/spyserver-client-16.h
	        ./spy-server-16/spy-handler-16.h
	        ./spy-server-16/tcp-client-16.h
	        ./spy-server-16/spyserver-protocol.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./spy-server-16/spyserver-client-16.cpp
	        ./spy-server-16/spy-handler-16.cpp
	        ./spy-server-16/tcp-client-16.cpp
	   )

	   add_definitions (-DHAVE_SPYSERVER_16)
	endif()

#
	if (RTL_TCP)
	   find_package (Qt6Network REQUIRED)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ./rtl_tcp/rtl_tcp-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ./rtl_tcp/rtl_tcp_client.h
	   )

	   include_directories (
	      ./rtl_tcp
	      ${Qt6Network_INCLUDE_DIRS}
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ./rtl_tcp/rtl_tcp_client.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ./rtl_tcp/rtl_tcp_client.cpp
	   )

	   set (RTLTCP_lib Qt6::Network)
	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTL_TCP)

	add_library(${devicesLibName} STATIC ${${devicesLibName}_SRCS})

	target_link_libraries(${devicesLibName}
        PRIVATE
        Qt6::Widgets
        Qt6::Xml
        Qt6::Network
        ${extraLibs}
)

target_include_directories(${devicesLibName} INTERFACE .)
