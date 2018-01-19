cmake_minimum_required( VERSION 2.8.11 )
set (objectName qt-dab-1.0-alpha)
set (CMAKE_CXX_FLAGS "${CMAKE_XCC_FLAGS} -Wall -std=c++11 -flto")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

set(CMAKE_AUTORCC ON)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()
#set (CMAKE_INSTALL_PREFIX /usr/local/bin)

#	modify if you want

if(MINGW)
    add_definitions ( -municode)
endif()

IF(EXISTS (".git") AND IS_DIRECTORY (".git"))
   execute_process (
      COMMAND git rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_COMMIT_HASH
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )
endif ()

if(GIT_COMMIT_HASH)
   add_definitions("-DGITHASH=\"${GIT_COMMIT_HASH}\"")
else ()
   add_definitions ("-DGITHASH=\"       \"")
endif()


########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#
#####################################################################
#
#	default value
	set(USE_PORTAUDIO true)

if(DEFINED AIRSPY)
   set(AIRSPY true)
endif ()

if(DEFINED SDRPLAY)
   set(SDRPLAY true)
endif ()

if(DEFINED RTLSDR)
   set(RTLSDR true)
endif ()

if(DEFINED RTLTCP)
   set(RTLTCP true)
endif ()

if (DEFINED SPECTRUM)
   set(SPECTRUM true)
endif ()

if (DEFINED DATA_STREAMER)
   set(DATA_STREAMER true)
endif ()

if (DEFINED IMPULSE_RESPONSE)
   set (IMPULSE_RESPONSE true)
endif ()

#	If you want a tcp stream of the audio rather than
#	audio being sent to the soundcard, set
if (DEFINED TCP_STREAMER)
	set(TCP_STREAMER true)
	set(USE_PORTAUDIO false)
endif ()

#	If you want the Qt system to handle the audio (only default out)
#	then set
if (DEFINED QT_AUDIO)
	set(QT_AUDIO true)
	set(USE_PORTAUDIO false)
endif ()

add_definitions (-DMSC_DATA__)		# use at your own risk
add_definitions (-DPRESET_NAME)
#add_definitions (-DTHREADED_DECODING)	# uncomment for use for an RPI
add_definitions (-DTII_GUESSING)
#
########################################################################

	find_package (PkgConfig)
	find_package (LSBId)

	find_package (Qt5Core REQUIRED)
	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Network REQUIRED)
	include_directories (
	  ${Qt5Network_INCLUDE_DIRS}
	)

        find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()

        find_package(Faad)
        if (NOT FAAD_FOUND )
            message(FATAL_ERROR "please install libfaad")
        endif ()

        find_package(zlib)
	if (NOT ZLIB_FOUND)
            message(FATAL_ERROR "please install libz")
        endif ()
	list(APPEND extraLibs ${ZLIB_LIBRARY})

        find_package(LibSndFile)
        if (NOT LIBSNDFILE_FOUND)
            message(FATAL_ERROR "please install libsndfile")
        endif ()
        list(APPEND extraLibs ${LIBSNDFILE_LIBRARY})

        find_package(LibSampleRate)
        if (NOT LIBSAMPLERATE_FOUND)
            message(FATAL_ERROR "please install libsamplerate")
        endif ()
        list(APPEND extraLibs ${LIBSAMPLERATE_LIBRARY})

	find_library (PTHREADS pthread)
	if (NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else (NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif (NOT(PTHREADS))

#######################################################################
#
#	Here we really start

	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           ${QT5Widgets_INCLUDES}
	           ${QT_QTCORE_INCLUDE_DIR}
	           ${QT_QTGUI_INCLUDE_DIR}
	           .
	           ./
	           ./includes
	           ./includes/ofdm
	           ./includes/backend
	           ./includes/backend/viterbi_768
	           ./includes/backend/audio
	           ./includes/backend/data
	           ./includes/backend/data/journaline
	           ./includes/various
	           ./includes/output
	           ./includes/scopes-qwt6
	           ./devices
	           ./devices/rawfiles
	           ./devices/wavfiles
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./dab-processor.h
             ./includes/country-codes.h
             ./includes/ofdm/sample-reader.h
	     ./includes/dab-constants.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/phasetable.h
	     ./includes/ofdm/freq-interleaver.h
	     ./includes/ofdm/tii_table.h
	     ./includes/ofdm/tii_detector.h
	     ./includes/backend/viterbi_768/viterbi-768.h
	     ./includes/backend/protection.h
	     ./includes/backend/uep-protection.h
	     ./includes/backend/eep-protection.h
	     ./includes/backend/firecode-checker.h
	     ./includes/backend/frame-processor.h
	     ./includes/backend/dab-virtual.h
	     ./includes/backend/charsets.h
	     ./includes/backend/galois.h
	     ./includes/backend/msc-handler.h
	     ./incluces/backend/reed-solomon.h
	     ./includes/backend/audio/dab-audio.h
	     ./includes/backend/audio/faad-decoder.h
	     ./includes/backend/audio/mp4processor.h
	     ./includes/backend/audio/mp2processor.h
	     ./includes/backend/data/ip-datahandler.h
	     ./includes/backend/data/tdc-datahandler.h
	     ./includes/backend/data/journaline-datahandler.h
	     ./includes/backend/data/journaline/dabdatagroupdecoder.h
	     ./includes/backend/data/journaline/crc_8_16.h
	     ./includes/backend/data/journaline/log.h
	     ./includes/backend/data/journaline/newssvcdec_impl.h
	     ./includes/backend/data/journaline/Splitter.h
	     ./includes/backend/data/journaline/dabdgdec_impl.h
	     ./includes/backend/data/journaline/newsobject.h
	     ./includes/backend/data/journaline/NML.h
	     ./includes/backend/data/mot-databuilder.h
	     ./includes/backend/data/virtual-datahandler.h
	     ./includes/backend/data/pad-handler.h
	     ./includes/backend/data/mot-data.h
	     ./includes/backend/data/dab-data.h
	     ./includes/backend/data/data-processor.h
	     ./devices/virtual-input.h
	     ./devices/rawfiles/rawfiles.h
	     ./devices/wavfiles/wavfiles.h
	     ./includes/output/fir-filters.h
	     ./includes/output/audio-base.h
	     ./includes/output/newconverter.h
	     ./includes/various/fft-handler.h
	     ./includes/various/ringbuffer.h
	     ./includes/various/Xtan2.h
	     ./includes/various/dab-params.h
	     ./includes/various/band-handler.h
	     ./includes/various/text-mapper.h
	     ./includes/various/ensemble-printer.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./dab-processor.cpp
             ./src/ofdm/sample-reader.cpp
	     ./src/ofdm/ofdm-decoder.cpp
	     ./src/ofdm/phasereference.cpp
	     ./src/ofdm/phasetable.cpp
	     ./src/ofdm/freq-interleaver.cpp
	     ./src/ofdm/tii_table.cpp
	     ./src/ofdm/tii_detector.cpp
	     ./src/backend/viterbi_768/viterbi-768.cpp
	     ./src/backend/viterbi_768/spiral-no-sse.c
	     ./src/backend/fic-handler.cpp
	     ./src/backend/msc-handler.cpp
	     ./src/backend/protection.cpp
	     ./src/backend/eep-protection.cpp
	     ./src/backend/uep-protection.cpp
	     ./src/backend/fib-processor.cpp
	     ./src/backend/firecode-checker.cpp
	     ./src/backend/dab-virtual.cpp
	     ./src/backend/frame-processor.cpp
	     ./src/backend/protTables.cpp
	     ./src/backend/charsets.cpp
	     ./src/backend/dab-virtual.cpp
	     ./src/backend/galois.cpp
	     ./src/backend/reed-solomon.cpp
	     ./src/backend/audio/dab-audio.cpp
	     ./src/backend/audio/faad-decoder.cpp
	     ./src/backend/audio/mp4processor.cpp
	     ./src/backend/audio/mp2processor.cpp
	     ./src/backend/data/ip-datahandler.cpp
	     ./src/backend/data/journaline-datahandler.cpp
	     ./src/backend/data/journaline/crc_8_16.c
	     ./src/backend/data/journaline/log.c
	     ./src/backend/data/journaline/newssvcdec_impl.cpp
	     ./src/backend/data/journaline/Splitter.cpp
	     ./src/backend/data/journaline/dabdgdec_impl.c
	     ./src/backend/data/journaline/newsobject.cpp
	     ./src/backend/data/journaline/NML.cpp
	     ./src/backend/data/mot-databuilder.cpp
	     ./src/backend/data/virtual-datahandler.cpp
	     ./src/backend/data/tdc-datahandler.cpp
	     ./src/backend/data/pad-handler.cpp
	     ./src/backend/data/mot-data.cpp
	     ./src/backend/data/dab-data.cpp
	     ./src/backend/data/data-processor.cpp
	     ./devices/virtual-input.cpp
	     ./devices/rawfiles/rawfiles.cpp
	     ./devices/wavfiles/wavfiles.cpp
	     ./src/output/audio-base.cpp
	     ./src/output/newconverter.cpp
	     ./src/output/fir-filters.cpp
	     ./src/various/fft-handler.cpp
	     ./src/various/Xtan2.cpp
	     ./src/various/dab-params.cpp
	     ./src/various/band-handler.cpp
	     ./src/various/text-mapper.cpp
	     ./src/various/ensemble-printer.cpp
	)

	set (${objectName}_MOCS
	     ./dab-processor.h
	     ./includes/output/audio-base.h
	     ./includes/ofdm/sample-reader.h
	     ./includes/ofdm/ofdm-decoder.h
	     ./includes/ofdm/phasereference.h
	     ./includes/backend/fic-handler.h
	     ./includes/backend/fib-processor.h
	     ./includes/backend/audio/faad-decoder.h
	     ./includes/backend/audio/mp2processor.h
	     ./includes/backend/audio/mp4processor.h
	     ./includes/backend/data/virtual-datahandler.h
	     ./includes/backend/data/pad-handler.h
	     ./includes/backend/data/mot-data.h
	     ./includes/backend/data/ip-datahandler.h
	     ./includes/backend/data/tdc-datahandler.h
	     ./includes/backend/data/journaline-datahandler.h
	     ./includes/backend/data/mot-databuilder.h
	     ./includes/backend/data/dab-data.h
	     ./includes/backend/data/data-processor.h
	     ./devices/rawfiles/rawfiles.h
	     ./devices/wavfiles/wavfiles.h
	     ./includes/ofdm/tii_detector.h
	     ./includes/ofdm/tii_table.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS} ./forms/dabradio.ui ./forms/technical_data.ui)

	set (${objectName}_MOCS
	        ${${objectName}_MOCS} ./radio.h)

	set ($(objectName)_HDRS
	        ${${objectName}_HDRS} ./radio.h)

	set (${objectName}_SRCS
	        ${${objectName}_SRCS} ./radio.cpp)


##########################################################################
#	The devices
#
	if (SDRPLAY)
#	   find_path (SDRPLAYLIB_INCLUDE_DIR
#                      NAMES mirsdrapi-rsp.h
#                      PATHS
#                      /usr/local/include/
#	   )
#	   include_directories (${SDRPLAYLIB_INCLUDE_DIR})
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/sdrplay-handler/sdrplay-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/sdrplay-handler/sdrplay-handler.h
	        ./devices/sdrplay-handler/sdrplayselect.h
	   )

	   include_directories (
	     ./devices/sdrplay-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/sdrplay-handler/sdrplay-handler.h
	        ./devices/sdrplay-handler/sdrplayselect.h
	        ./devices/sdrplay-handler/mirsdrapi-rsp.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/sdrplay-handler/sdrplay-handler.cpp
	        ./devices/sdrplay-handler/sdrplayselect.cpp
	   )

	 add_definitions (-DHAVE_SDRPLAY)
	endif (SDRPLAY)

	if (AIRSPY)
#           find_package(LibAIRSPY)
#           if (NOT LIBAIRSPY_FOUND)
#               message(FATAL_ERROR "please install airspy library")
#           endif ()
#	   ## include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/airspy-handler/airspy-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/airspy-handler/airspy-handler.h
	   )

	   include_directories (
	     ./devices/airspy-handler
	     ./devices/airspy-handler/libairspy
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/airspy-handler/airspy-handler.h
	        ./devices/airspy-handler/airspyfilter.h
	        ./devices/airspy-handler/libairspy/airspy.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/airspy-handler/airspy-handler.cpp
	        ./devices/airspy-handler/airspyfilter.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY)
	endif (AIRSPY)

#
	if (RTLSDR)
           find_package(LibRTLSDR)
           if (NOT LIBRTLSDR_FOUND)
               message(FATAL_ERROR "please install librtlsdr")
           endif ()
	   ###include_directories (${RTLSDR_INCLUDE_DIR})

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/rtlsdr-handler/rtlsdr-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/rtlsdr-handler/rtl-dongleselect.h
	        ./devices/rtlsdr-handler/rtlsdr-handler.h
	   )

	   include_directories (
	        ./devices/rtlsdr-handler/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.h
	        ./devices/rtlsdr-handler/rtl-dongleselect.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtlsdr-handler/rtlsdr-handler.cpp
	        ./devices/rtlsdr-handler/rtl-dongleselect.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()
#
	if (TCP_STREAMER)
	   add_definitions (-DTCP_STREAMER)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./includes/output/tcp-streamer.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./includes/output/tcp-streamer.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/output/tcp-streamer.cpp
	   )
	endif (TCP_STREAMER)

	if (QT_AUDIO)
	   add_definitions (-DQT_AUDIO)
	   find_package (Qt5Multimedia REQUIRED)
	   include_directories (
	      ${Qt5Multimedia_INCLUDE_DIRS}
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./includes/output/Qt-audio.h
	        ./includes/output/Qt-audiodevice.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./includes/output/Qt-audio.h
	        ./includes/output/Qt-audiodevice.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./src/output/Qt-audio.cpp
	        ./src/output/Qt-audiodevice.cpp
	   )
	endif (QT_AUDIO)

	if (RTLTCP)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/rtl_tcp/rtl_tcp-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/rtl_tcp/rtl_tcp_client.h
	   )

	   include_directories (
	      ./devices/rtl_tcp
	      ${Qt5Network_INCLUDE_DIRS}
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/rtl_tcp/rtl_tcp_client.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/rtl_tcp/rtl_tcp_client.cpp
	   )

	   set (RTLTCP_lib Qt5::Network)
	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTLTCP)

	if (SPECTRUM)
	   find_package(Qwt)
           if (NOT QWT_FOUND)
               message(FATAL_ERROR "please install qwt library")
           endif ()
	   include_directories ( ${QWT_INCLUDE_DIRS})
           list(APPEND extraLibs ${QWT_LIBRARIES})
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./optional-scope/scopewidget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./optional-scope/spectrum-handler.h
	        ./includes/scopes-qwt6/iqdisplay.h
	   )

	   include_directories (
	      ./optional-scope
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./optional-scope/spectrum-handler.h
	        ./includes/scopes-qwt6/iqdisplay.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./optional-scope/spectrum-handler.cpp
	        ./src/scopes-qwt6/iqdisplay.cpp
	   )

	   add_definitions (-DHAVE_SPECTRUM)
	   add_definitions (-D__QUALITY)
	endif (SPECTRUM)

	if (TRY_EPG)
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	             ./includes/backend/data/epg/epgdec.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	             ./src/backend/data/epg/epgdec.cpp
	   )
	   add_definitions (-DTRY_EPG)
	endif (TRY_EPG)

	if (DATA_STREAMER)
	   include_directories (
	      ./server-thread
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	             ./server-thread/tcp-server.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	             ./server-thread/tcp-server.cpp
	   )
	   add_definitions (-DDATA_STREAMER)
	endif (DATA_STREAMER)

	if (IMPULSE_RESPONSE)
	   include_directories (
	      ./optional-ir
	   )

	   set (${objectName}_HDRS 
	      ${${objectName}_HDRS}
	           ./optional-ir/impulse-viewer.h
	   )

	   set (${objectName}_MOCS 
	      ${${objectName}_MOCS}
	           ./optional-ir/impulse-viewer.h
	   )

	   set (${objectName}_SRCS
	      ${${objectName}_SRCS}
	           ./optional-ir/impulse-viewer.cpp
	   )

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./optional-ir/impulse-widget.ui
	   )

	   add_definitions (-DIMPULSE_RESPONSE)
	endif (IMPULSE_RESPONSE)
	
	if (USE_PORTAUDIO)
           find_package(Portaudio)
           if (NOT PORTAUDIO_FOUND)
               message(FATAL_ERROR "please install portaudio V19")
           endif ()
           list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	       ./includes/output/audiosink.h
	   )

	   set (${objectName}_MOCS
	       ${${objectName}_MOCS}
	      ./includes/output/audiosink.h
	   )

	   set (${objectName}_SRCS
	      ${${objectName}_SRCS}
	      ./src/output/audiosink.cpp
	   )
	endif (USE_PORTAUDIO)

	QT5_WRAP_UI (UIS ${${objectName}_UIS}
	             ./devices/filereader-widget.ui)

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT5Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
	          ${FFTW_INCLUDE_DIRS}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	          ${SNDFILES_INCLUDE_DIRS}
	)

	QT5_WRAP_CPP (MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${RSCS}
	                ${TRS}
	                ${MOCS}
	                resources.qrc
	)

	target_link_libraries (${objectName}
	                       Qt5::Widgets
	                       Qt5::Network
	                       ${RTLTCP_lib}
	                       ${FFTW3F_LIBRARIES}
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	)
	if (QT_AUDIO)
	   target_link_libraries (${objectName} Qt5::Multimedia)
	endif (QT_AUDIO)

	INSTALL (TARGETS ${objectName} DESTINATION  ${CMAKE_INSTALL_PREFIX})
#	INSTALL (TARGETS ${objectName} DESTINATION ./linux-bin)

########################################################################
# Create uninstall target
########################################################################

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
