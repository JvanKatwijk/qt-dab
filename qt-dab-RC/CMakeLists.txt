cmake_minimum_required( VERSION 3.21 )
set (objectName qt-dab-6.5Beta)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -g -fsanitize=address")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -fsanitize=address")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17 -flto")
#set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

set(CMAKE_AUTORCC ON)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()
#set (CMAKE_INSTALL_PREFIX /usr/local/bin)

#	modify if you want
#	Note that since there are always problems with the rtlsdr interface
#	under Windows we split the rtlsdr interface into two versions
#	one for Linux and one for Window

if(MINGW)
	add_definitions ( -municode)
#else (MINGW)
#	add_definitions (-DSSE_AVAILABLE)
endif()

add_definitions (-D__FFTW3__ -D__ESTIMATOR__)

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.git" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../.git")
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
#Select ONE of the following options
set (USE_PORTAUDIO ON)
set (QT_AUDIO OFF)	# not recently tested
set (TCP_STREAMER OFF)

#####################################################################
#

# select one or more devices (set to ON)
# Note that since there are problems with the rtlsdr interface under Windows we split the rtlsdr interface
# into two versions one for Linux and one for Window
option(RTLSDR_LINUX OFF)	# use for Linux type systems
option(RTLSDR_WIN OFF) 		# only for windows
option(AIRSPY OFF)  
option(SDRPLAY_V2 OFF) 
option(SDRPLAY_V3 OFF)
option(HACKRF OFF)
option(LIMESDR OFF) 
option(PLUTO OFF) 
option(SOAPY OFF)
option(SPY_SERVER_8 OFF)
option(SPY_SERVER_16 OFF)
option(UHD OFF) 
option(RTL_TCP OFF)      


# additional options
option(DATA_STREAMER OFF)
option(VITERBI_SSE OFF)
option(VITERBI_NEON OFF)
option(FDK_AAC OFF)


####################################################################
#	Use first option on "slow" cpus, i.e. RPI 3 or so
#	Most of the FFT processing will be done in functions eunning
#	in its own thread
#add_definitions (-D__MSC_THREAD__)	# do not uncomment this
#
#	Since more than one backend may be active, activating this
#	option makes that each backend runs in its own thread
add_definitions (-D__THREADED_BACKEND)	# uncomment for use for an RPI
#
########################################################################
########################################################################

	find_package (PkgConfig)
	find_package (LSBId)

	find_package (Qt5Core REQUIRED)
	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Network REQUIRED)
	find_package (Qt5Xml REQUIRED)
	include_directories (
	  ${Qt5Network_INCLUDE_DIRS}
	  ${Qt5Xml_INCLUDE_DIRS}
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
	include_directories ( ${FFTW_INCLUDE_DIRS})
        list(APPEND extraLibs ${FFTW3F_LIBRARIES})


	pkg_check_modules (ZLIB zlib)
	if (ZLIB_FOUND)
	   list  (APPEND extraLibs ${ZLIB_LDFLAGS})
	else ()
           find_package(zlib)
	   if (NOT ZLIB_FOUND)
               message(FATAL_ERROR "please install libz")
           endif ()
	   list(APPEND extraLibs ${ZLIB_LIBRARY})
	endif ()
	include_directories (${ZLIB_INCLUDE_DIRS})

	pkg_check_modules (LIBSNDFILE sndfile)
	if (LIBSNDFILE_FOUND)
	   list (APPEND extraLibs ${LIBSNDFILE_LDFLAGS})
	else ()
           find_package(LibSndFile)
           if (NOT LIBSNDFILE_FOUND)
              message(FATAL_ERROR "please install libsndfile")
           endif ()
           list(APPEND extraLibs ${LIBSNDFILE_LIBRARY})
	endif ()
	include_directories (${LIBSNDFILE_LIBRARY})

	pkg_check_modules (LIBSAMPLERATE samplerate)
	if (LIBSAMPLERATE_FOUND)
	   list (APPEND extraLibs ${LIBSAMPLERATE_LDFLAGS})
	else ()
           find_package(LibSampleRate)
           if (NOT LIBSAMPLERATE_FOUND)
              message(FATAL_ERROR "please install libsamplerate")
           endif ()
           list(APPEND extraLibs ${LIBSAMPLERATE_LIBRARY})
	endif ()
	include_directories (${LIBSAMPLERATE_INCLUDE_DIRS})

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
	           ./pauzeslide
	           ./support
	           ..
	           ../includes
	           ../includes/ofdm
	           ../includes/protection
	           ../includes/backend
	           ../includes/backend/audio
	           ../includes/backend/data
	           ../includes/backend/data/journaline
	           ../includes/backend/data/mot
	           ../includes/backend/data/epg-2
	           ../includes/backend/data/epg
	           ../includes/support
	           ../src/support/tii-library
	           ../includes/support/buttons
	           ../includes/support/viterbi-jan
	           ../includes/support/viterbi-spiral
	           ../includes/output
	           ../includes/scopes-qwt6
	           ./forms-v7/new-display
	           ./forms-v7/new-display/scopes
	           ../viewers
	           ../viewers/snr-viewer
	           ../qt-devices
	           ../qt-devices/rawfiles-new
	           ../qt-devices/wavfiles-new
	           ../qt-devices/xml-filereader
	           ../eti-handler
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./radio.h
	     ./support/techdata.h 
             ./support/clickable-label.h 
             ./support/icon-label.h 
             ./support/scan-handler.h 
             ./support/skiptable-handler.h 
             ./support/aboutdialog.h 
	     ./support/ensemble-handler.h
	     ./forms-v7/new-display/display-widget.h 
             ./forms-v7/new-display/scopes/correlation-scope.h 
             ./forms-67/new-display/scopes/spectrum-scope.h 
             ./forms-v7/new-display/scopes/null-scope.h 
             ./forms-v7/new-display/scopes/channel-scope.h 
	     ./forms-v7/new-display/dev-scope.h
             ./forms-v7/new-display/scopes/waterfall-scope.h 
             ./forms-v7/new-display/scopes/iqdisplay.h 
	     ./forms-v7/new-display/scopes/audio-display.h
	     ../ofdm-handler.h
	     ../eti-handler/eti-generator.h
	     ../includes/dab-constants.h
	     ../includes/crc-handlers.h
	     ../includes/bit-extractors.h
	     ../includes/mot-content-types.h 
             ../includes/ofdm/sample-reader.h
	     ../includes/ofdm/correlator.h
	     ../includes/ofdm/freqsyncer.h
	     ../includes/ofdm/ofdm-decoder.h
	     ../includes/ofdm/phasetable.h
	     ../includes/ofdm/freq-interleaver.h
	     ../includes/ofdm/fib-decoder.h
	     ../includes/ofdm/dab-config.h
	     ../includes/ofdm/fib-table.h
	     ../includes/ofdm/fic-handler.h
	     ../includes/ofdm/tii-detector.h
	     ../includes/ofdm/timesyncer.h
	     ../includes/ofdm/estimator.h
	     ../includes/protection/protTables.h
	     ../includes/protection/protection.h
	     ../includes/protection/uep-protection.h
	     ../includes/protection/eep-protection.h
	     ../includes/backend/firecode-checker.h
	     ../includes/backend/frame-processor.h
	     ../includes/backend/charsets.h
	     ../includes/backend/galois.h
	     ../incluces/backend/reed-solomon.h
	     ../includes/backend/msc-handler.h
	     ../includes/backend/backend.h
	     ../includes/backend/backend-deconvolver.h
	     ../includes/backend/backend-driver.h
	     ../includes/backend/audio/mp4processor.h
	     ../includes/backend/audio/bitWriter.h
	     ../includes/backend/audio/mp2processor.h
	     ../includes/backend/data/ip-datahandler.h
	     ../includes/backend/data/tdc-datahandler.h
	     ../includes/backend/data/journaline-datahandler.h
	     ../includes/backend/data/journaline/dabdatagroupdecoder.h
	     ../includes/backend/data/journaline/crc_8_16.h
	     ../includes/backend/data/journaline/log.h
	     ../includes/backend/data/journaline/newssvcdec_impl.h
	     ../includes/backend/data/journaline/Splitter.h
	     ../includes/backend/data/journaline/dabdgdec_impl.h
	     ../includes/backend/data/journaline/newsobject.h
	     ../includes/backend/data/journaline/NML.h
	     ../includes/backend/data/epg/epgdec.h
	     ../includes/backend/data/epg-2/epg-decoder.h
	     ../includes/backend/data/virtual-datahandler.h
	     ../includes/backend/data/pad-handler.h
	     ../includes/backend/data/mot/mot-handler.h
	     ../includes/backend/data/mot/mot-object.h
	     ../includes/backend/data/mot/mot-dir.h
	     ../includes/backend/data/data-processor.h
	     ../includes/output/audio-player.h
	     ../includes/output/newconverter.h
	     ../includes/support/fft-handler.h 
	     ../includes/support/converter_48000.h 
	     ../includes/support/ringbuffer.h
	     ../includes/support/Xtan2.h
	     ../includes/support/dab-params.h
	     ../includes/support/dab-tables.h
	     ../includes/support/viterbi-jan/viterbi-handler.h
	     ../includes/support/viterbi-spiral/viterbi-spiral.h
	     ../includes/support/preset-handler.h
	     ../includes/support/presetcombobox.h
	     ../includes/support/scanlist-handler.h
	     ../includes/support/scheduler.h
	     ../includes/support/schedule-selector.h
	     ../includes/support/element-selector.h
	     ../includes/support/time-table.h
	     ../includes/support/findfilenames.h
	     ../includes/support/content-table.h
	     ../includes/support/dl-cache.h
	     ../includes/support/ITU_Region_1.h
	     ../includes/support/coordinates.h
	     ../includes/support/skin-handler.h
	     ../includes/support/mapport.h
	     ../includes/support/upload.h
	     ,,.includes/support/http-handler.h
	     ,,.includes/support/converted_map.h
	     ../includes/support/fir-filters.h
	     ../includes/support/tii-mapper.h
	     ../src/support/tii-library/table-loader.h
	     ../includes/support/font-chooser.h 
	     ../includes/support/buttons/smallcombobox.h 
             ../includes/support/buttons/newpushbutton.h 
             ../includes/support/buttons/normalpushbutton.h 
             ../includes/support/buttons/smallpushbutton.h 
	     ../includes/support/buttons/verysmallpushbutton.h 
             ../includes/support/buttons/smallqlistview.h 
             ../includes/support/buttons/smallspinbox.h 
	     ../includes/scopes-qwt6/spectrogramdata.h
	     ../viewers/snr-viewer/snr-viewer.h
	     ../qt-devices/device-handler.h
	     ../qt-devices/device-chooser.h
	     ../qt-devices/device-exceptions.h
	     ../qt-devices/xml-filewriter.h
	     ../qt-devices/rawfiles-new/rawfiles.h
	     ../qt-devices/rawfiles-new/raw-reader.h
	     ../qt-devices/wavfiles-new/wavfiles.h
	     ../qt-devices/wavfiles-new/wav-reader.h
	     ../qt-devices/xml-filereader/element-reader.h
	     ../qt-devices/xml-filereader/xml-filereader.h
	     ../qt-devices/xml-filereader/xml-reader.h
	     ../qt-devices/xml-filereader/xml-descriptor.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./main.cpp
	     ./radio.cpp
	     ./support/techdata.cpp
	     ./support/clickable-label.cpp 
             ./support/icon-label.cpp 
             ./support/scan-handler.cpp 
             ./support/skiptable-handler.cpp 
             ./support/aboutdialog.cpp 
             ./support/ensemble-handler.cpp 
	     ./forms-v7/new-display/display-widget.cpp 
             ./forms-v7/new-display/scopes/spectrum-scope.cpp 
             ./forms-v7/new-display/scopes/correlation-scope.cpp 
             ./forms-v7/new-display/scopes/null-scope.cpp 
             ./forms-v7/new-display/scopes/channel-scope.cpp 
             ./forms-v7/new-display/scopes/dev-scope.cpp 
             ./forms-v7/new-display/scopes/waterfall-scope.cpp 
             ./forms-v7/new-display/scopes/iqdisplay.cpp 
	     ./forms-v7/new-display/scopes/audio-display.cpp
	     ./forms-v7/new-display/scopes/audio-display.cpp 
	     ../ofdm-handler.cpp
	     ../eti-handler/eti-generator.cpp
             ../src/ofdm/sample-reader.cpp
	     ../src/ofdm/ofdm-decoder.cpp
	     ../src/ofdm/correlator.cpp
	     ../src/ofdm/freqsyncer.cpp
	     ../src/ofdm/phasetable.cpp
	     ../src/ofdm/freq-interleaver.cpp
	     ../src/ofdm/fib-decoder.cpp
	     ../src/ofdm/fic-handler.cpp
	     ../src/ofdm/tii-detector.cpp
	     ../src/ofdm/timesyncer.cpp
	     ../src/ofdm/estimator.cpp
	     ../src/protection/protTables.cpp
	     ../src/protection/protection.cpp
	     ../src/protection/eep-protection.cpp
	     ../src/protection/uep-protection.cpp
	     ../src/backend/firecode-checker.cpp
	     ../src/backend/charsets.cpp
	     ../src/backend/galois.cpp
	     ../src/backend/reed-solomon.cpp
	     ../src/backend/msc-handler.cpp
	     ../src/backend/backend.cpp
	     ../src/backend/backend-deconvolver.cpp
	     ../src/backend/backend-driver.cpp
	     ../src/backend/audio/mp4processor.cpp
	     ../src/backend/audio/bitWriter.cpp
	     ../src/backend/audio/mp2processor.cpp
	     ../src/backend/data/ip-datahandler.cpp
	     ../src/backend/data/journaline-datahandler.cpp
	     ../src/backend/data/journaline/crc_8_16.c
	     ../src/backend/data/journaline/log.c
	     ../src/backend/data/journaline/newssvcdec_impl.cpp
	     ../src/backend/data/journaline/Splitter.cpp
	     ../src/backend/data/journaline/dabdgdec_impl.c
	     ../src/backend/data/journaline/newsobject.cpp
	     ../src/backend/data/journaline/NML.cpp
	     ../src/backend/data/epg-2/epg-decoder.cpp
	     ../src/backend/data/epg/epgdec.cpp
	     ../src/backend/data/tdc-datahandler.cpp
	     ../src/backend/data/pad-handler.cpp
	     ../src/backend/data/mot/mot-handler.cpp
	     ../src/backend/data/mot/mot-object.cpp
	     ../src/backend/data/mot/mot-dir.cpp
	     ../src/backend/data/data-processor.cpp
	     ../src/output/audio-player.cpp
	     ../src/output/newconverter.cpp
	     ../src/support/converter_48000.cpp
	     ../src/support/viterbi-jan/viterbi-handler.cpp
	     ../src/support/viterbi-spiral/viterbi-spiral.cpp
	     ../src/support/tii-library/table-loader.cpp
	     ../src/support/fft-handler.cpp
	     ../src/support/dab-params.cpp
#	     ../src/support/band-handler.cpp
	     ../src/support/dab-tables.cpp
	     ../src/support/preset-handler.cpp
	     ../src/support/presetcombobox.cpp
	     ../src/support/scanlist-handler.cpp
	     ../src/support/scheduler.cpp
	     ../src/support/schedule-selector.cpp
	     ../src/support/element-selector.cpp
	     ../src/support/time-table.cpp
	     ../src/support/findfilenames.cpp
	     ../src/support/content-table.cpp
	     ../src/support/ITU_Region_1.cpp
	     ../src/support/coordinates.cpp
	     ../src/support/skin-handler.cpp
	     ../src/support/mapport.cpp
	     ../src/support/http-handler.cpp
	     ../src/support/upload.cpp
	     ../src/support/bandpass-filter.cpp
	     ../src/support/fir-filters.cpp
	     ../src/support/font-chooser.cpp
	     ../src/support/tii-mapper.cpp
	     ../src/support/buttons/smallcombobox.cpp
	     ../src/support/buttons/newpushbutton.cpp
	     ../src/support/buttons/normalpushbutton.cpp
	     ../src/support/buttons/smallpushbutton.cpp
	     ../src/support/buttons/verysmallpushbutton.cpp
	     ../src/support/buttons/smallqlistview.cpp
	     ../src/support/buttons/smallspinbox.cpp
	     ../src/scopes-qwt6/spectrogramdata.cpp
	     ../src/scopes-qwt6/spectrogramdata.cpp 
	     ../viewers/snr-viewer/snr-viewer.cpp
	     ../qt-devices/device-handler.cpp
	     ../qt-devices/device-chooser.cpp
	     ../qt-devices/xml-filewriter.cpp
	     ../qt-devices/rawfiles-new/rawfiles.cpp
	     ../qt-devices/rawfiles-new/raw-reader.cpp
	     ../qt-devices/wavfiles-new/wavfiles.cpp
	     ../qt-devices/wavfiles-new/wav-reader.cpp
	     ../qt-devices/xml-filereader/xml-filereader.cpp
	     ../qt-devices/xml-filereader/xml-reader.cpp
	     ../qt-devices/xml-filereader/xml-descriptor.cpp
	)

	set (${objectName}_MOCS
	     ./radio.h
	     ./support/techdata.h
	     ./support/clickable-label.h 
             ./support/icon-label.h 
             ./support/scan-handler.h 
             ./support/skiptable-handler.h 
             ./support/aboutdialog.h 
             ./support/ensemble-handler.h 
	     ./forms-v7/new-display/display-widget.h 
             ./forms-v7/new-display/scopes/correlation-scope.h 
             ./forms-v7/new-display/scopes/spectrum-scope.h 
             ./forms-v7/new-display/scopes/null-scope.h 
             ./forms-v7/new-display/scopes/channel-scope.h 
             ./forms-v7/new-display/scopes/dev-scope.h 
             ./forms-v7/new-display/scopes/waterfall-scope.h 
             ./forms-v7/new-display/scopes/iqdisplay.h 
	     ../ofdm-handler.h
	     ../includes/ofdm/sample-reader.h
	     ../includes/ofdm/ofdm-decoder.h
	     ../includes/ofdm/correlator.h
	     ../includes/ofdm/freqsyncer.h
	     ../includes/ofdm/fib-decoder.h
	     ../includes/ofdm/fic-handler.h
	     ../includes/ofdm/tii-detector.h
	     ../includes/ofdm/estimator.h
	     ../includes/backend/msc-handler.h
	     ../includes/backend/backend.h
	     ../includes/backend/audio/mp2processor.h
	     ../includes/backend/audio/mp4processor.h
	     ../includes/backend/data/virtual-datahandler.h
	     ../includes/backend/data/pad-handler.h
	     ../includes/backend/data/mot/mot-handler.h
	     ../includes/backend/data/mot/mot-object.h
	     ../includes/backend/data/mot/mot-dir.h
	     ../includes/backend/data/ip-datahandler.h
	     ../includes/backend/data/tdc-datahandler.h
	     ../includes/backend/data/journaline-datahandler.h
             ../includes/backend/data/epg-2/epg-decoder.h
	     ../includes/backend/data/data-processor.h
	     ../includes/output/audio-player.h
	     ../includes/support/converter_48000.h
	     ../includes/support/preset-handler.h
	     ../includes/support/presetcombobox.h
	     ../includes/support/font-chooser.h
	     ../includes/support/buttons/smallcombobox.h 
	     ../includes/support/buttons/newpushbutton.h 
	     ../includes/support/buttons/normalpushbutton.h 
	     ../includes/support/buttons/smallpushbutton.h 
	     ../includes/support/buttons/verysmallpushbutton.h 
	     ../includes/support/buttons/smallqlistview.h 
	     ../includes/support/buttons/smallspinbox.h 
	     ../includes/support/scanlist-handler.h
	     ../includes/support/scheduler.h
	     ../includes/support/schedule-selector.h
	     ../includes/support/element-selector.h
	     ../includes/support/time-table.h
	     ../includes/support/content-table.h
	     ../includes/support/coordinates.h
	     ../includes/support/skin-handler.h
	     ../includes/support/mapport.h
	     ../includes/support/upload.h
	     ../includes/support/http-handler.h
	     ../includes/scopes-qwt6/audio-display.h
 	     ../viewers/snr-viewer/snr-viewer.h
	     ../qt-devices/rawfiles-new/rawfiles.h
	     ../qt-devices/rawfiles-new/raw-reader.h
	     ../qt-devices/wavfiles-new/wavfiles.h
	     ../qt-devices/wavfiles-new/wav-reader.h
	     ../qt-devices/xml-filereader/xml-filereader.h
	     ../qt-devices/xml-filereader/xml-reader.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	     ./support/aboutdialog.ui
	     ./forms-v7/dabradio-6.ui
	     ./forms-v7/technical_data.ui
	     ./forms-v7/config-helper.ui
	     ./forms-v7/new-display/scopewidget.ui
	     ../viewers/snr-viewer/snr-widget.ui
	     ../qt-devices/xml-filereader/xmlfiles.ui
	   )


#########################################################################

           find_package(Faad)
           if (NOT FAAD_FOUND )
              message(FATAL_ERROR "please install libfaad")
           endif ()
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../includes/backend/audio/faad-decoder.h
	   )
	   set (${objectName}_SRCS
                ${${objectName}_SRCS}
	        ../src/backend/audio/faad-decoder.cpp
	   )
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../includes/backend/audio/faad-decoder.h
	   )
	   add_definitions (-D__WITH_FAAD__)

#	the viterbi decoder
#
	if (VITERBI_NEON)
	   set(VITERBI_SSE false)
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-neon.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-neon.c
	   )
	   
	   add_definitions (-DNEON_AVAILABLE)
	elseif(VITERBI_SSE)
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-sse.c
	   )
	   add_definitions (-DSSE_AVAILABLE)
	else (VITERBI_SSE)
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../src/support/viterbi-spiral/spiral-no-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/support/viterbi-spiral/spiral-no-sse.c
	   )
	endif (VITERBI_NEON)

##########################################################################
#	The devices
#
	if (SDRPLAY_V2)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-widget-v2.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.h
	   )

	   include_directories (
	     ../qt-devices/sdrplay-handler-v2
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.h
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.h
	        ../qt-devices/sdrplay-handler-v2/mirsdrapi-rsp.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/sdrplay-handler-v2/sdrplay-handler-v2.cpp
	        ../qt-devices/sdrplay-handler-v2/sdrplayselect.cpp
	   )

	   add_definitions (-DHAVE_SDRPLAY_V2)
	endif (SDRPLAY_V2)

	if (SDRPLAY)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-widget-v3.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h
	        ../qt-devices/sdrplay-handler-v3/sdrplay-commands.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp-device.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.h
	   )

	   include_directories (
	     ../qt-devices/sdrplay-handler-v3
	     ../qt-devices/sdrplay-handler-v3/include
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.h
	        ../qt-devices/sdrplay-handler-v3/sdrplay-commands.h
	        ../qt-devices/sdrplay-handler-v3/include/sdrplay_api.h
	        ../qt-devices/sdrplay-handler-v3/Rsp-device.h 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.h 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.h
           )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/sdrplay-handler-v3/sdrplay-handler-v3.cpp
                ../qt-devices/sdrplay-handler-v3/Rsp-device.cpp 
	        ../qt-devices/sdrplay-handler-v3/Rsp1A-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspII-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspDuo-handler.cpp 
	        ../qt-devices/sdrplay-handler-v3/RspDx-handler.cpp 
	   )

	 add_definitions (-DHAVE_SDRPLAY_V3)
	endif (SDRPLAY)

	if (AIRSPY)
#           find_package(LibAIRSPY)
#           if (NOT LIBAIRSPY_FOUND)
#               message(FATAL_ERROR "please install airspy library")
#           endif ()
#	   ## include_directories (${AIRSPYLIB_INCLUDE_DIR})

	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/airspy-handler/airspy-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/airspy-handler/airspy-handler.h
	        ../qt-devices/airspy-handler/airspyselect.h
	   )

	   include_directories (
	     ../qt-devices/airspy-handler
	     ../qt-devices/airspy-handler/libairspy
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/airspy-handler/airspy-handler.h
	        ../qt-devices/airspy-handler/airspyselect.h
	        ../qt-devices/airspy-handler/libairspy/airspy.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/airspy-handler/airspy-handler.cpp
	        ../qt-devices/airspy-handler/airspyselect.cpp
	   )

	   add_definitions (-DHAVE_AIRSPY)
	endif (AIRSPY)

	if (HACKRF)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/hackrf-handler/hackrf-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/hackrf-handler/hackrf-handler.h
	   )

	   include_directories (
	     ../qt-devices/hackrf-handler
	     ../qt-devices/hackrf-handler/libhackrf
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/hackrf-handler/hackrf-handler.h
	        ../qt-devices/hackrf-handler/libhackrf/hackrf.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/hackrf-handler/hackrf-handler.cpp
	   )

	   add_definitions (-DHAVE_HACKRF)
	endif (HACKRF)
#
	if (PLUTO)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/pluto-handler/pluto-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/pluto-handler/pluto-handler.h
	   )

	   include_directories (
	     ../qt-devices/pluto-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/pluto-handler/pluto-handler.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/pluto-handler/pluto-handler.cpp
	   )

	   add_definitions (-DHAVE_PLUTO)
	endif (PLUTO)
#
	if (LIMESDR)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/lime-handler/lime-handler.h
	        ../qt-devices/lime-handler/lime-widget.h
	   )

	   include_directories (
	     ../qt-devices/lime-handler
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/lime-handler/lime-handler.h
	        ../qt-devices/lime-handler/lime-widget.h
	        ../qt-devices/lime-handler/LimeSuite.h
	        ../qt-devices/lime-handler/LMS7002M_parameters.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/lime-handler/lime-handler.cpp
	   )

	   add_definitions (-DHAVE_LIME)
	endif (LIMESDR)

	if (RTLSDR_LINUX)
	   set (${objectName}_UIS
                ${${objectName}_UIS}
                ../qt-devices/rtlsdr-handler-linux/rtlsdr-widget.ui
           )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.h
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.h
	   )

	   include_directories (
	        ../qt-devices/rtlsdr-handler-linux/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-sdr.h
	        ../qt-devices/rtlsdr-handler-linux/rtl-sdr_export.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/rtlsdr-handler-linux/rtlsdr-handler.cpp
	        ../qt-devices/rtlsdr-handler-linux/rtl-dongleselect.cpp
	   )

	   add_definitions (-DHAVE_RTLSDR)
	endif()

	if (SOAPY)
	   set (${objectName}_UIS
                ${${objectName}_UIS}
                ../qt-devices/soapy/soapy-widget.ui
           )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/soapy/soapy-handler.h
	        ../qt-devices/soapy/soapy-converter.h
	   )

	   include_directories (
	        ../qt-devices/soapy/
	   )

	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/soapy/soapy-handler.h
	        ../qt-devices/soapy/soapy-converter.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
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

	   set (${objectName}_UIS
                ${${objectName}_UIS}
                ../qt-devices/spy-server-8/spyserver-widget-8.ui
           )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/spy-server-8/spyserver-client-8.h
	        ../qt-devices/spy-server-8/spy-handler-8.h
	        ../qt-devices/spy-server-8/tcp-client-8.h
	   )


	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/spy-server-8/spyserver-client-8.h
	        ../qt-devices/spy-server-8/spy-handler-8.h
	        ../qt-devices/spy-server-8/tcp-client-8.h
	        ../qt-devices/spy-server-8/spyserver-protocol.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
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

	   set (${objectName}_UIS
                ${${objectName}_UIS}
                ../qt-devices/spy-server-16/spyserver-widget-16.ui
           )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/spy-server-16/spyserver-client-16.h
	        ../qt-devices/spy-server-16/spy-handler-16.h
	        ../qt-devices/spy-server-16/tcp-client-16.h
	   )


	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/spy-server-16/spyserver-client-16.h
	        ../qt-devices/spy-server-16/spy-handler-16.h
	        ../qt-devices/spy-server-16/tcp-client-16.h
	        ../qt-devices/spy-server-16/spyserver-protocol.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/spy-server-16/spyserver-client-16.cpp
	        ../qt-devices/spy-server-16/spy-handler-16.cpp
	        ../qt-devices/spy-server-16/tcp-client-16.cpp
	   )

	   add_definitions (-DHAVE_SOAPY_16)
	endif()

#
	if (TCP_STREAMER)
	   add_definitions (-DTCP_STREAMER)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../includes/output/tcp-streamer.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../includes/output/tcp-streamer.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/output/tcp-streamer.cpp
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
	        ../includes/output/Qt-audio.h
	        ../includes/output/Qt-audiodevice.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ../includes/output/Qt-audio.h
	        ../includes/output/Qt-audiodevice.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../src/output/Qt-audio.cpp
	        ../src/output/Qt-audiodevice.cpp
	   )
	endif (QT_AUDIO)

	if (RTL_TCP)
	   find_package (Qt5Network REQUIRED)
	   set (${objectName}_UIS
	        ${${objectName}_UIS}
	        ../qt-devices/rtl_tcp/rtl_tcp-widget.ui
	   )

	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.h
	   )

	   include_directories (
	      ../qt-devices/rtl_tcp
	      ${Qt5Network_INCLUDE_DIRS}
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ../qt-devices/rtl_tcp/rtl_tcp_client.cpp
	   )

	   set (RTLTCP_lib Qt5::Network)
	   add_definitions (-DHAVE_RTL_TCP)
	endif (RTL_TCP)

	if (DATA_STREAMER)
	   include_directories (
	      ../server-thread
	   )

	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	             ../server-thread/tcp-server.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	             ../server-thread/tcp-server.cpp
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
	       ../includes/output/audiosink.h
	   )

	   set (${objectName}_MOCS
	       ${${objectName}_MOCS}
	       ../includes/output/audiosink.h
	   )

	   set (${objectName}_SRCS
	      ${${objectName}_SRCS}
	      ../src/output/audiosink.cpp
	   )
	endif (USE_PORTAUDIO)

	QT5_WRAP_UI (UIS ${${objectName}_UIS}
	             ../qt-devices/filereader-widget.ui)

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT5Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
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
	                       Qt5::Xml
	                       ${RTLTCP_lib}
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
