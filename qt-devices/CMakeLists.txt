
set(devicesLibName ${objectName}_devices)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)

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
        ${QT5Widgets_INCLUDES}
        ${QT_QTCORE_INCLUDE_DIR}
        ${QT_QTGUI_INCLUDE_DIR}
        ${QT5Widgets_INCLUDES}
        ${QT_QTCORE_INCLUDE_DIR}
	filereaders
	filereaders/xml-filereader
        filereaders/rawfiles-new
        filereaders/wavfiles-new
)

	if (SDRPLAY_V2)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-widget-v2.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.h
	   )

	   include_directories (
	     ../qt-devices/sdrplay-handler-v2
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.h
	        ../qt-devices/sdrplay-handler-v2/mirsdrapi-rsp.h
           )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.cpp
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.cpp
	   )

	   add_definitions (-DHAVE_SDRPLAY_V2)
	endif (SDRPLAY_V2)

	if (SDRPLAY_V3)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-widget-v3.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h
	        ../qt-devices/sdrplay-handler-v3/sdrplay-commands.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp-device.h 
	        ../qt-devices/sdrplay-handler-v3/RspI-handler.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.h
	   )

	   include_directories (
	     ../qt-devices/sdrplay-handler-v3
	     ../qt-devices/sdrplay-handler-v3/include
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h
	        ../qt-devices/sdrplay-handler-v3/sdrplay-commands.h
	        ../qt-devices/sdrplay-handler-v3/include/sdrplay_api.h
	        ../qt-devices/sdrplay-handler-v3/Rsp-device.h 
	        ../qt-devices/sdrplay-handler-v3/RspI-handler.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.h
           )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.cpp
                ../qt-devices/sdrplay-handler-v3/Rsp-device.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspI-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.cpp 
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
	        ../qt-devices/airspy-2/airspy-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/airspy-2/airspy-2.h
	        ../qt-devices/airspy-2/airspyselect.h
	   )

	   include_directories (
	     ../qt-devices/airspy-2
	     ../qt-devices/airspy-2/libairspy
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/airspy-2/airspy-2.h
	        ../qt-devices/airspy-2/airspyselect.h
	        ../qt-devices/airspy-2/libairspy/airspy.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/airspy-2/airspy-2.cpp
	        ../qt-devices/airspy-2/airspyselect.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY_2)
	endif (AIRSPY)

	if (HACKRF)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ../qt-devices/hackrf-handler/hackrf-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/hackrf-handler/hackrf-handler.h
	   )

	   include_directories (
	     ../qt-devices/hackrf-handler
	     ../qt-devices/hackrf-handler/libhackrf
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/hackrf-handler/hackrf-handler.h
	        ../qt-devices/hackrf-handler/libhackrf/hackrf.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/hackrf-handler/hackrf-handler.cpp
	   )

	   add_definitions (-DHAVE_HACKRF)
	endif (HACKRF)
#
	if (PLUTO)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ../qt-devices/pluto-handler/pluto-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/pluto-handler/pluto-handler.h
	   )

	   include_directories (
	     ../qt-devices/pluto-handler
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/pluto-handler/pluto-handler.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/pluto-handler/pluto-handler.cpp
	   )

	   add_definitions (-DHAVE_PLUTO)
	endif (PLUTO)
#
	if (LIMESDR)
	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/lime-handler/lime-handler.h
	        ../qt-devices/lime-handler/lime-widget.h
	   )

	   include_directories (
	     ../qt-devices/lime-handler
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/lime-handler/lime-handler.h
	        ../qt-devices/lime-handler/lime-widget.h
	        ../qt-devices/lime-handler/LimeSuite.h
	        ../qt-devices/lime-handler/LMS7002M_parameters.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/lime-handler/lime-handler.cpp
	   )

	   add_definitions (-DHAVE_LIME)
	endif (LIMESDR)

	if (RTLSDR_LINUX)
	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ../qt-devices/rtlsdr-handler-linux/rtlsdr-widget.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.h
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.h
	   )

	   include_directories (
	        ../qt-devices/rtlsdr-handler-linux/
	   )

	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-sdr.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-sdr_export.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.cpp
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()

	if (SOAPY)
	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ../qt-devices/soapy/soapy-widget.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/soapy/soapy-handler.h
	        ../qt-devices/soapy/soapy-converter.h
	   )

	   include_directories (
	        ../qt-devices/soapy/
	   )

	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/soapy/soapy-handler.h
	        ../qt-devices/soapy/soapy-converter.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/soapy/soapy-handler.cpp
	        ../qt-devices/soapy/soapy-converter.cpp
	   )

	   set (LIBSOAPY "-lSoapy -lm")
	   list(APPEND extraLibs ${LIBSOAPY})
	   add_definitions (-DHAVE_SOAPY)
	endif()

	if (SPY_SERVER_8)
	   include_directories (
	        ../qt-devices/spy-server-8/
	   )

	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ../qt-devices/spy-server-8/spyserver-widget-8.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/spy-server-8/spyserver-client-8.h
	        ../qt-devices/spy-server-8/spy-handler-8.h
	        ../qt-devices/spy-server-8/tcp-client-8.h
	   )


	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/spy-server-8/spyserver-client-8.h
	        ../qt-devices/spy-server-8/spy-handler-8.h
	        ../qt-devices/spy-server-8/tcp-client-8.h
	        ../qt-devices/spy-server-8/spyserver-protocol.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/spy-server-8/spyserver-client-8.cpp
	        ../qt-devices/spy-server-8/spy-handler-8.cpp
	        ../qt-devices/spy-server-8/tcp-client-8.cpp
	   )

	   add_definitions (-DHAVE_SOAPY_8)
	endif()

	if (SPY_SERVER_16)
	   include_directories (
	        ../qt-devices/spy-server-16/
	   )

	   set (${devicesLibName}_UIS
                ${${devicesLibName}_UIS}
                ../qt-devices/spy-server-16/spyserver-widget-16.ui
           )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/spy-server-16/spyserver-client-16.h
	        ../qt-devices/spy-server-16/spy-handler-16.h
	        ../qt-devices/spy-server-16/tcp-client-16.h
	   )


	   set (${devicesLibName}_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/spy-server-16/spyserver-client-16.h
	        ../qt-devices/spy-server-16/spy-handler-16.h
	        ../qt-devices/spy-server-16/tcp-client-16.h
	        ../qt-devices/spy-server-16/spyserver-protocol.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/spy-server-16/spyserver-client-16.cpp
	        ../qt-devices/spy-server-16/spy-handler-16.cpp
	        ../qt-devices/spy-server-16/tcp-client-16.cpp
	   )

	   add_definitions (-DHAVE_SOAPY_16)
	endif()

#
	if (RTL_TCP)
	   find_package (Qt5Network REQUIRED)
	   set (${devicesLibName}_UIS
	        ${${devicesLibName}_UIS}
	        ../qt-devices/rtl_tcp/rtl_tcp-widget.ui
	   )

	   set (${devicesLibName}_MOCS
	        ${${devicesLibName}_MOCS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.h
	   )

	   include_directories (
	      ../qt-devices/rtl_tcp
	      ${Qt5Network_INCLUDE_DIRS}
	   )

	   set ($(devicesLibName)_HDRS
	        ${${devicesLibName}_HDRS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.h
	   )

	   set (${devicesLibName}_SRCS
	        ${${devicesLibName}_SRCS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.cpp
	   )

	   set (RTLTCP_lib Qt5::Network)
	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTL_TCP)

	add_library(${devicesLibName} STATIC ${${devicesLibName}_SRCS})

	target_link_libraries(${devicesLibName}
        PRIVATE
        Qt5::Widgets
        Qt5::Xml
        Qt5::Network
        ${extraLibs}
)

target_include_directories(${devicesLibName} INTERFACE .)
