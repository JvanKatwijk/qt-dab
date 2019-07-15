cmake_minimum_required( VERSION 2.8.11 )
set (objectName qt-dab-2.7.1)
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

if(DEFINED HACKRF)
	set(HACKRF true)
endif ()

if(DEFINED RTLTCP)
   set(RTLTCP true)
endif ()

if(DEFINED LIMESDR)
   set(LIMESDR true)
endif ()


if (DEFINED DATA_STREAMER)
   set(DATA_STREAMER true)
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

	find_package(Qwt)
	if (NOT QWT_FOUND)
	   message(FATAL_ERROR "please install qwt library")
	endif ()
	include_directories ( ${QWT_INCLUDE_DIRS})
	list(APPEND extraLibs ${QWT_LIBRARIES})

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
	           ./service-description
	           ./includes
	           ./includes/ofdm
	           ./includes/protection
	           ./includes/backend
	           ./includes/backend/audio
	           ./includes/backend/data
	           ./includes/backend/data/journaline
	           ./includes/backend/data/mot
	           ./includes/support
	           ./includes/support/viterbi-jan
	           ./includes/output
	           ./includes/scopes-qwt6
	           ./devices
	           ./devices/rawfiles
	           ./devices/wavfiles
	           ./spectrum-viewer
	           ./impulse-viewer
	           ./tii-viewer
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./dab-processor.h
	     ./service-description/service-descriptor.h 
	     ./service-description/audio-descriptor.h 
	     ./service-description/data-descriptor.h 
             ./includes/country-codes.h
	     ./includes/dab-constants.h
             ./includes/ofdm/sample-reader.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/ofdm-decoder.h
	     ./includes/ofdm/phasetable.h
	     ./includes/ofdm/freq-interleaver.h
	     ./includes/ofdm/fib-decoder.h
	     ./includes/ofdm/fic-handler.h
	     ./includes/ofdm/tii_table.h
	     ./includes/ofdm/tii_detector.h
	     ./includes/ofdm/timesyncer.h
	     ./includes/protection/protTables.h
	     ./includes/protection/protection.h
	     ./includes/protection/uep-protection.h
	     ./includes/protection/eep-protection.h
	     ./includes/backend/firecode-checker.h
	     ./includes/backend/frame-processor.h
	     ./includes/backend/charsets.h
	     ./includes/backend/galois.h
	     ./incluces/backend/reed-solomon.h
	     ./includes/backend/msc-handler.h
	     ./includes/backend/backend.h
	     ./includes/backend/backend-deconvolver.h
	     ./includes/backend/backend-driver.h
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
	     ./includes/backend/data/virtual-datahandler.h
	     ./includes/backend/data/pad-handler.h
	     ./includes/backend/data/mot/mot-handler.h
	     ./includes/backend/data/mot/mot-object.h
	     ./includes/backend/data/mot/mot-dir.h
	     ./includes/backend/data/data-processor.h
	     ./devices/virtual-input.h
	     ./devices/rawfiles/rawfiles.h
	     ./devices/rawfiles/raw-reader.h
	     ./devices/wavfiles/wavfiles.h
	     ./devices/wavfiles/wav-reader.h
	     ./includes/output/fir-filters.h
	     ./includes/output/audio-base.h
	     ./includes/output/newconverter.h
	     ./includes/support/fft-handler.h
	     ./includes/support/ringbuffer.h
	     ./includes/support/Xtan2.h
	     ./includes/support/dab-params.h
	     ./includes/support/band-handler.h
	     ./includes/support/text-mapper.h
	     ./includes/support/dab_tables.h
	     ./includes/support/ensemble-printer.h
	     ./includes/support/viterbi-jan/viterbi-handler.h
	     ./includes/scopes-qwt6/iqdisplay.h
	     ./spectrum-viewer/spectrum-viewer.h 
	     ./impulse-viewer/impulse-viewer.h 
	     ./tii-viewer/tii-viewer.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./dab-processor.cpp
	     ./service-description/audio-descriptor.cpp
	     ./service-description/data-descriptor.cpp
             ./src/ofdm/sample-reader.cpp
	     ./src/ofdm/ofdm-decoder.cpp
	     ./src/ofdm/phasereference.cpp
	     ./src/ofdm/phasetable.cpp
	     ./src/ofdm/freq-interleaver.cpp
	     ./src/ofdm/fib-decoder.cpp
	     ./src/ofdm/fic-handler.cpp
	     ./src/ofdm/tii_table.cpp
	     ./src/ofdm/tii_detector.cpp
	     ./src/ofdm/timesyncer.cpp
	     ./src/protection/protTables.cpp
	     ./src/protection/protection.cpp
	     ./src/protection/eep-protection.cpp
	     ./src/protection/uep-protection.cpp
	     ./src/backend/firecode-checker.cpp
	     ./src/backend/frame-processor.cpp
	     ./src/backend/charsets.cpp
	     ./src/backend/galois.cpp
	     ./src/backend/reed-solomon.cpp
	     ./src/backend/msc-handler.cpp
	     ./src/backend/backend.cpp
	     ./src/backend/backend-deconvolver.cpp
	     ./src/backend/backend-driver.cpp
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
	     ./src/backend/data/virtual-datahandler.cpp
	     ./src/backend/data/tdc-datahandler.cpp
	     ./src/backend/data/pad-handler.cpp
	     ./src/backend/data/mot/mot-handler.cpp
	     ./src/backend/data/mot/mot-object.cpp
	     ./src/backend/data/mot/mot-dir.cpp
	     ./src/backend/data/data-processor.cpp
	     ./devices/virtual-input.cpp
	     ./devices/rawfiles/rawfiles.cpp
	     ./devices/rawfiles/raw-reader.cpp
	     ./devices/wavfiles/wavfiles.cpp
	     ./devices/wavfiles/wav-reader.cpp
	     ./src/output/audio-base.cpp
	     ./src/output/newconverter.cpp
	     ./src/output/fir-filters.cpp
	     ./src/support/fft-handler.cpp
	     ./src/support/Xtan2.cpp
	     ./src/support/dab-params.cpp
	     ./src/support/band-handler.cpp
	     ./src/support/text-mapper.cpp
	     ./src/support/dab_tables.cpp
	     ./src/support/ensemble-printer.cpp
	     ./src/support/viterbi-jan/viterbi-handler.cpp
	     ./src/scopes-qwt6/iqdisplay.cpp 
	     ./spectrum-viewer/spectrum-viewer.cpp 
	     ./impulse-viewer/impulse-viewer.cpp 
	     ./tii-viewer/tii-viewer.cpp
	)

	set (${objectName}_MOCS
	     ./dab-processor.h
	     ./includes/output/audio-base.h
	     ./includes/output/audiosink.h
	     ./includes/ofdm/sample-reader.h
	     ./includes/ofdm/ofdm-decoder.h
	     ./includes/ofdm/phasereference.h
	     ./includes/ofdm/fib-decoder.h
	     ./includes/ofdm/fic-handler.h
	     ./includes/ofdm/tii_detector.h
	     ./includes/ofdm/tii_table.h
	     ./includes/backend/backend.h
	     ./includes/backend/audio/faad-decoder.h
	     ./includes/backend/audio/mp2processor.h
	     ./includes/backend/audio/mp4processor.h
	     ./includes/backend/data/virtual-datahandler.h
	     ./includes/backend/data/pad-handler.h
	     ./includes/backend/data/mot/mot-handler.h
	     ./includes/backend/data/mot/mot-object.h
	     ./includes/backend/data/mot/mot-dir.h
	     ./includes/backend/data/ip-datahandler.h
	     ./includes/backend/data/tdc-datahandler.h
	     ./includes/backend/data/journaline-datahandler.h
	     ./includes/backend/data/data-processor.h
	     ./devices/rawfiles/rawfiles.h
	     ./devices/rawfiles/raw-reader.h
	     ./devices/wavfiles/wavfiles.h
	     ./devices/wavfiles/wav-reader.h
	     ./includes/scopes-qwt6/iqdisplay.h
	     ./spectrum-viewer/spectrum-viewer.h 
	     ./impulse-viewer/impulse-viewer.h 
 	     ./tii-viewer/tii-viewer.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	     ./forms/dabradio.ui
	     ./forms/technical_data.ui
	     ./forms/audio-description.ui
	     ./forms/data-description.ui
	     ./spectrum-viewer/scopewidget.ui
	     ./impulse-viewer/impulse-widget.ui
	     ./tii-viewer/tii-widget.ui
	   )

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

	if (HACKRF)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/hackrf-handler/hackrf-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/hackrf-handler/hackrf-handler.h
	   )

	   include_directories (
	     ./devices/hackrf-handler
	     ./devices/hackrf-handler/libhackrf
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/hackrf-handler/hackrf-handler.h
	        ./devices/hackrf-handler/libhackrf/hackrf.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/hackrf-handler/hackrf-handler.cpp
	   )

	   add_definitions (-DHAVE_HACKRF)
	endif (HACKRF)
#
	if (LIMESDR)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ./devices/lime-handler/lime-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./devices/lime-handler/lime-handler.h
	   )

	   include_directories (
	     ./devices/lime-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./devices/lime-handler/lime-handler.h
	        ./devices/lime-handler/LimeSuite.h
	        ./devices/lime-handler/LMS7002M_parameters.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./devices/lime-handler/lime-handler.cpp
	   )

	   add_definitions (-DHAVE_LIME)
	endif (LIMESDR)

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
