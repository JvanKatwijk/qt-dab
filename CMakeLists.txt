cmake_minimum_required( VERSION 3.21 )

# set CMAKE_BUILD_TYPE to "Release" if not defined (like call from command line)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Set build type to Release if not specified" FORCE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/Modules/CMakeFunctions.cmake)

project(Qt-DAB VERSION 6)
set (objectName qt-dab-6.9.2)
set (objectName "Qt-DAB")
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/local/bin" CACHE PATH "default install path" FORCE )
endif()

add_definitions(-DAPP_NAME="${objectName}")
add_definitions(-DPRJ_NAME="${PROJECT_NAME}")
add_definitions(-DPRJ_VERS="${PROJECT_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # regarding using QWT < 6.2.0 (6.1.4) to avoid many deprecated massages (only suppressed on release versions)
    add_definitions(-DQT_NO_DEPRECATED_WARNINGS)
endif()

if(MINGW)
	add_definitions ( -municode)
endif()

add_definitions (-D__HAS_CHANNEL__)
add_definitions (-D__FFTW3__)

# handle GIT hash
get_git_commit_hash() # returns GIT_COMMIT_HASH
print(GIT_COMMIT_HASH)
if (GIT_COMMIT_HASH)
    add_definitions("-DGITHASH=\"${GIT_COMMIT_HASH}\"")
else ()
    add_definitions("-DGITHASH=\"(unknown)\"")
endif ()


########################################################################
# select the release build type by default to get optimization flags
########################################################################
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

### make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

######################################################################
#
#	if you want the audio output sent over the internet:
set (TCP_STREAMER OFF)

#####################################################################
#
# select one or more devices (set to ON)
# Note that since there are problems with the rtlsdr interface under Windows we split the rtlsdr interface
# into two versions one for Linux and one for Window
option(RTLSDR_LINUX OFF)	# use for Linux type systems
option(RTLSDR_WIN OFF) 		# only for windows
option(AIRSPY OFF)  
option(SDRPLAY_V2 ON) 
option(SDRPLAY_V3 OFF)
option(HACKRF OFF)
option(LIMESDR OFF) 
option(PLUTO OFF) 
option(SOAPY OFF)
option(SPY_SERVER_8 OFF)
option(SPY_SERVER_16 OFF)
option(UHD OFF) 		# untested
option(RTL_TCP OFF)      

# additional options
option(USE_LTO "Set -flto flag" ON)  
# special release version flags
if (USE_LTO)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif ()

option(FDK_AAC ON)
option(DATA_STREAMER OFF)
option(VITERBI_SSE OFF)
option(VITERBI_NEON OFF)


####################################################################
#	Use first option on "slow" cpus, i.e. RPI 3 or so
#	Most of the FFT processing will be done in functions eunning
#	in its own thread
#add_definitions (-D__MSC_THREAD__)	# do not uncomment this
#
#	Since more than one backend may be active, activating this
#	option makes that each backend runs in its own thread
add_definitions (-D__THREADED_BACKEND__)	# uncomment for use for an RPI
#
########################################################################
########################################################################

	find_package (PkgConfig)
	find_package (LSBId)

	find_package (Qt6Core REQUIRED)
	find_package (Qt6Widgets REQUIRED)
	find_package (Qt6Network REQUIRED)
	find_package (Qt6Xml REQUIRED)
	include_directories (
	  ${Qt6Network_INCLUDE_DIRS}
	  ${Qt6Xml_INCLUDE_DIRS}
	)

	search_for_library (QWT Qwt)
	search_for_library (ZLIB zlib)

	find_package(FFTW3f)
        if (NOT FFTW3F_FOUND)
            message(FATAL_ERROR "please install FFTW3")
        endif ()
	include_directories ( ${FFTW_INCLUDE_DIRS})
        list(APPEND extraLibs ${FFTW3F_LIBRARIES})

	if (NOT APPLE)
	   find_library (PTHREADS pthread)
	   if (NOT(PTHREADS))
	      message (FATAL_ERROR "please install libpthread")
	   else (NOT(PTHREADS))
	      set (extraLibs ${extraLibs} ${PTHREADS})
	   endif (NOT(PTHREADS))
	endif (NOT APPLE)

#######################################################################
#
#	Here we really start

	include_directories (
	           ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	           ${QT6Widgets_INCLUDES}
	           ${QT_QTCORE_INCLUDE_DIR}
	           ${QT_QTGUI_INCLUDE_DIR}
	           ./sources/main
	           ./sources/main/forms-v7
	           ./sources/main/forms-v7/new-display
	           ./sources/main/forms-v7/new-display/scopes
	           ./sources/main/forms-v7/snr-viewer
	           ./sources/eti-handler
	           ./sources/support
	           ./sources/support/gui-elements
	           ./sources/frontend
	           ./sources/backend
	           ./sources/backend/audio
	           ./sources/backend/data
	           ./sources/backend/data/journaline
	           ./sources/backend/data/mot
	           ./sources/backend/data/epg
	           ./sources/backend/reed-solomon
	           ./sources/protection
	           ./sources/output
	           ./sources/output/portaudio
	           ./sources/output/Qt6
	           ./sources/devices
	           ./sources/devices/filereaders/
	           ./sources/devices/filereaders/new-reader
	           ./sources/devices/filereaders/rawfiles-new
	           ./sources/devices/filereaders/xml-filereader
	           /usr/include/
	)

	set (${objectName}_HDRS
	     ./sources/main/radio.h
	     ./sources/main/aboutdialog.h
	     ./sources/main/config-handler.h
	     ./sources/main/ensemble-handler.h
	     ./sources/main/techdata.h
	     ./sources/main/forms-v7/new-display/display-widget.h 
             ./sources/main/forms-v7/new-display/scopes/correlation-scope.h 
             ./sources/main/forms-v7/new-display/scopes/spectrum-scope.h 
             ./sources/main/forms-v7/new-display/scopes/null-scope.h 
             ./sources/main/forms-v7/new-display/scopes/channel-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/dev-scope.h
             ./sources/main/forms-v7/new-display/scopes/waterfall-scope.h 
             ./sources/main/forms-v7/new-display/scopes/iqdisplay.h 
	     ./sources/main/forms-v7/new-display/scopes/audio-display.h
	     ./sources/main/forms-v7/new-display/scopes/spectrogramdata.h
	     ./sources/main/forms-v7/snr-viewer/snr-viewer.h
	     ./sources/eti-handler/eti-generator.h
	     ./sources/frontend/ofdm-handler.h
             ./sources/frontend/sample-reader.h
	     ./sources/frontend/correlator.h
	     ./sources/frontend/freqsyncer.h
	     ./sources/frontend/ofdm-decoder.h
	     ./sources/frontend/phasetable.h
	     ./sources/frontend/freq-interleaver.h
	     ./sources/frontend/fic-handler.h
	     ./sources/frontend/ensemble.h
	     ./sources/frontend/fib-config.h
	     ./sources/frontend/fib-decoder.h
	     ./sources/frontend/fib-table.h
	     ./sources/frontend/tii-detector.h
	     ./sources/frontend/tii-detector-1.h
	     ./sources/frontend/tii-detector-2.h
	     ./sources/frontend/timesyncer.h
	     ./sources/frontend/estimator.h
	     ./sources/protection/protTables.h
	     ./sources/protection/protection.h
	     ./sources/protection/uep-protection.h
	     ./sources/protection/eep-protection.h
	     ./sources/support/audiosystem-selector.h
	     ./sources/support/bandpass-filter.h
	     ./sources/support/bit-extractor.h
	     ./sources/support/db-element.h
	     ./sources/support/charsets.h
	     ./sources/support/content-table.h
	     ./sources/support/converter_48000.h
	     ./sources/support/coordinates.h
	     ./sources/support/country-codes.h
	     ./sources/support/crc-handlers.h
	     ./sources/support/dab-constants.h
	     ./sources/support/dab-params.h
	     ./sources/support/dab-tables.h
	     ./sources/support/distances.h
	     ./sources/support/dl-cache.h
	     ./sources/support/dxDisplay.h
	     ./sources/support/element-selector.h
	     ./sources/support/fft-handler.h
	     ./sources/support/findfilenames.h
	     ./sources/support/fir-filters.h
	     ./sources/support/http-handler.h
	     ./sources/support/ITU_Region_1.h
	     ./sources/support/logger.h
	     ./sources/support/mapport.h
	     ./sources/support/mot-content-types.h
	     ./sources/support/position-handler.h
	     ./sources/support/presetcombobox.h
	     ./sources/support/preset-handler.h
	     ./sources/support/process-params.h
	     ./sources/support/ringbuffer.h
	     ./sources/support/scan-handler.h
	     ./sources/support/scanlist-handler.h
	     ./sources/support/scantable-handler.h
	     ./sources/support/scheduler.h
	     ./sources/support/schedule-selector.h
	     ./sources/support/settingNames.h
	     ./sources/support/settings-handler.h
	     ./sources/support/skin-handler.h
	     ./sources/support/tii-mapper.h
	     ./sources/support/tii-reader.h
	     ./sources/support/time-converter.h
	     ./sources/support/time-table.h
	     ./sources/support/uploader.h
	     ./sources/support/wavWriter.h
	     ./sources/support/gui-elements/circular-button.h
	     ./sources/support/gui-elements/clickable-label.h
	     ./sources/support/gui-elements/color-selector.h
	     ./sources/support/gui-elements/font-chooser.h 
	     ./sources/support/gui-elements/icon-label.h 
             ./sources/support/gui-elements/newpushbutton.h 
             ./sources/support/gui-elements//normalpushbutton.h 
	     ./sources/support/gui-elements/smallcombobox.h 
             ./sources/support/gui-elements/smallpushbutton.h 
             ./sources/support/gui-elements/smallqlistview.h 
             ./sources/support/gui-elements/smallspinbox.h 
             ./sources/support/gui-elements/super-frame.h 
	     ./sources/support/gui-elements/verysmallpushbutton.h 
	     ./sources/backend/backend.h
	     ./sources/backend/backend-deconvolver.h
	     ./sources/backend/backend-driver.h
	     ./sources/backend/frame-processor.h
	     ./sources/backend/msc-handler.h
	     ./sources/backend/reed-solomon/galois.h
	     ./sources/backend/reed-solomon/reed-solomon.h
	     ./sources/backend/audio/bitWriter.h
	     ./sources/backend/audio/firecode-checker.h
	     ./sources/backend/audio/mp2processor.h
	     ./sources/backend/audio/mp4processor.h
	     ./sources/backend/audio/neaacdec.h
	     ./sources/backend/data/adv-datahandler.h
	     ./sources/backend/data/data-processor.h
	     ./sources/backend/data/ip-datahandler.h
	     ./sources/backend/data/journaline-datahandler.h
	     ./sources/backend/data/journaline-screen.h
	     ./sources/backend/data/pad-handler.h
	     ./sources/backend/data/tdc-datahandler.h
	     ./sources/backend/data/virtual-datahandler.h
	     ./sources/backend/data/epg/epg-compiler.h
	     ./sources/backend/data/epg/xml-extractor.h
	     ./sources/backend/data/journaline/cpplog.h
	     ./sources/backend/data/journaline/crc_8_16.h
	     ./sources/backend/data/journaline/dabdatagroupdecoder.h
	     ./sources/backend/data/journaline/dabdgdec_impl.h
	     ./sources/backend/data/journaline/log.h
	     ./sources/backend/data/journaline/newsobject.h
	     ./sources/backend/data/journaline/newssvcdec_impl.h
	     ./sources/backend/data/journaline/NML.h
	     ./sources/backend/data/journaline/Splitter.h
	     ./sources/backend/data/mot/mot-dir.h
	     ./sources/backend/data/mot/mot-handler.h
	     ./sources/backend/data/mot/mot-object.h
	     ./sources/output/audio-player.h
	     ./sources/devices/device-handler.h
	     ./sources/devices/device-exceptions.h
	     ./sources/qt-devicces/xml-filewriter.h
	)

	set (${objectName}_SRCS
	     ${${objectName}_SRCS}
	     ./sources/main/main.cpp
	     ./sources/main/radio.cpp
	     ./sources/main/aboutdialog.cpp
	     ./sources/main/config-handler.cpp
	     ./sources/main/ensemble-handler.cpp
	     ./sources/main/techdata.cpp
	     ./sources/main/forms-v7/new-display/display-widget.cpp 
             ./sources/main/forms-v7/new-display/scopes/correlation-scope.cpp 
             ./sources/main/forms-v7/new-display/scopes/spectrum-scope.cpp 
             ./sources/main/forms-v7/new-display/scopes/null-scope.cpp 
             ./sources/main/forms-v7/new-display/scopes/channel-scope.cpp 
	     ./sources/main/forms-v7/new-display/scopes/dev-scope.cpp
             ./sources/main/forms-v7/new-display/scopes/waterfall-scope.cpp 
             ./sources/main/forms-v7/new-display/scopes/iqdisplay.cpp 
	     ./sources/main/forms-v7/new-display/scopes/audio-display.cpp
	     ./sources/main/forms-v7/new-display/scopes/spectrogramdata.cpp
	     ./sources/main/forms-v7/snr-viewer/snr-viewer.cpp
	     ./sources/eti-handler/eti-generator.cpp
	     ./sources/frontend/ofdm-handler.cpp
             ./sources/frontend/sample-reader.cpp
	     ./sources/frontend/correlator.cpp
	     ./sources/frontend/freqsyncer.cpp
	     ./sources/frontend/ofdm-decoder.cpp
	     ./sources/frontend/phasetable.cpp
	     ./sources/frontend/freq-interleaver.cpp
	     ./sources/frontend/fic-handler.cpp
	     ./sources/frontend/ensemble.cpp
	     ./sources/frontend/fib-config.cpp
	     ./sources/frontend/fib-decoder.cpp
	     ./sources/frontend/tii-detector.cpp
	     ./sources/frontend/tii-detector-1.cpp
	     ./sources/frontend/tii-detector-2.cpp
	     ./sources/frontend/timesyncer.cpp
	     ./sources/frontend/estimator.cpp
	     ./sources/protection/protTables.cpp
	     ./sources/protection/protection.cpp
	     ./sources/protection/uep-protection.cpp
	     ./sources/protection/eep-protection.cpp
	     ./sources/support/audiosystem-selector.cpp
	     ./sources/support/charsets.cpp
	     ./sources/support/bandpass-filter.cpp
	     ./sources/support/content-table.cpp
	     ./sources/support/converter_48000.cpp
	     ./sources/support/coordinates.cpp
	     ./sources/support/dab-params.cpp
	     ./sources/support/dab-tables.cpp
	     ./sources/support/distances.cpp
	     ./sources/support/dxDisplay.cpp
	     ./sources/support/element-selector.cpp
	     ./sources/support/fft-handler.cpp
	     ./sources/support/findfilenames.cpp
	     ./sources/support/fir-filters.cpp
	     ./sources/support/http-handler.cpp
	     ./sources/support/ITU_Region_1.cpp
	     ./sources/support/logger.cpp
	     ./sources/support/mapport.cpp
	     ./sources/support/position-handler.cpp
	     ./sources/support/presetcombobox.cpp
	     ./sources/support/preset-handler.cpp
	     ./sources/support/scan-handler.cpp
	     ./sources/support/scanlist-handler.cpp
	     ./sources/support/scantable-handler.cpp
	     ./sources/support/scheduler.cpp
	     ./sources/support/schedule-selector.cpp
	     ./sources/support/settings-handler.cpp
	     ./sources/support/skin-handler.cpp
	     ./sources/support/tii-mapper.cpp
	     ./sources/support/tii-reader.cpp
	     ./sources/support/time-converter.cpp
	     ./sources/support/time-table.cpp
	     ./sources/support/uploader.cpp
	     ./sources/support/wavWriter.cpp
	     ./sources/support/gui-elements/circular-button.cpp
	     ./sources/support/gui-elements/clickable-label.cpp
	     ./sources/support/gui-elements/color-selector.cpp
	     ./sources/support/gui-elements/font-chooser.cpp 
	     ./sources/support/gui-elements/icon-label.cpp 
             ./sources/support/gui-elements/newpushbutton.cpp 
             ./sources/support/gui-elements//normalpushbutton.cpp 
	     ./sources/support/gui-elements/smallcombobox.cpp 
             ./sources/support/gui-elements/smallpushbutton.cpp 
             ./sources/support/gui-elements/smallqlistview.cpp 
             ./sources/support/gui-elements/smallspinbox.cpp 
             ./sources/support/gui-elements/super-frame.cpp 
	     ./sources/support/gui-elements/verysmallpushbutton.cpp 
	     ./sources/backend/backend.cpp
	     ./sources/backend/backend-deconvolver.cpp
	     ./sources/backend/backend-driver.cpp
	     ./sources/backend/msc-handler.cpp
	     ./sources/backend/reed-solomon/galois.cpp
	     ./sources/backend/reed-solomon/reed-solomon.cpp
	     ./sources/backend/audio/bitWriter.cpp
	     ./sources/backend/audio/faad-decoder.cpp
	     ./sources/backend/audio/firecode-checker.cpp
	     ./sources/backend/audio/mp2processor.cpp
	     ./sources/backend/audio/mp4processor.cpp
	     ./sources/backend/data/adv-datahandler.cpp
	     ./sources/backend/data/data-processor.cpp
	     ./sources/backend/data/ip-datahandler.cpp
	     ./sources/backend/data/journaline-datahandler.cpp
	     ./sources/backend/data/journaline-screen.cpp
	     ./sources/backend/data/pad-handler.cpp
	     ./sources/backend/data/tdc-datahandler.cpp
	     ./sources/backend/data/epg/epg-compiler.cpp
	     ./sources/backend/data/epg/xml-extractor.cpp
	     ./sources/backend/data/journaline/crc_8_16.c
	     ./sources/backend/data/journaline/dabdgdec_impl.c
	     ./sources/backend/data/journaline/log.c
	     ./sources/backend/data/journaline/newsobject.cpp
	     ./sources/backend/data/journaline/newssvcdec_impl.cpp
	     ./sources/backend/data/journaline/NML.cpp
	     ./sources/backend/data/journaline/Splitter.cpp
	     ./sources/backend/data/mot/mot-dir.cpp
	     ./sources/backend/data/mot/mot-handler.cpp
	     ./sources/backend/data/mot/mot-object.cpp
	     ./sources/output/audio-player.cpp
	     ./sources/devices/device-handler.cpp
	     ./sources/devices/xml-filewriter.cpp
	)

	set (${objectName}_MOCS
	     ./sources/main/radio.h
	     ./sources/main/ensemble-handler.h 
	     ./sources/main/config-handler.h 
	     ./sources/main/techdata.h
	     ./sources/main/aboutdialog.h 
	     ./sources/main/forms-v7/new-display/display-widget.h 
	     ./sources/main/forms-v7/new-display/scopes/correlation-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/spectrum-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/null-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/channel-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/dev-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/waterfall-scope.h 
	     ./sources/main/forms-v7/new-display/scopes/iqdisplay.h 
	     ./sources/main/forms-v7/new-display/scopes/audio-display.h
 	     ./sources/main/forms-v7/snr-viewer/snr-viewer.h
	     ./sources/frontend/ofdm-handler.h
	     ./sources/frontend/sample-reader.h
	     ./sources/frontend/ofdm-decoder.h
	     ./sources/frontend/correlator.h
	     ./sources/frontend/freqsyncer.h
	     ./sources/frontend/fic-handler.h
	     ./sources/frontend/fib-decoder.h
	     ./sources/frontend/tii-detector.h
	     ./sources/frontend/estimator.h
	     ./sources/support/gui-elements/clickable-label.h 
	     ./sources/support/gui-elements/color-selector.h 
	     ./sources/support/gui-elements/font-chooser.h
	     ./sources/support/gui-elements/icon-label.h 
	     ./sources/support/gui-elements/newpushbutton.h 
	     ./sources/support/gui-elements/normalpushbutton.h 
	     ./sources/support/gui-elements/smallcombobox.h 
	     ./sources/support/gui-elements/smallpushbutton.h 
	     ./sources/support/gui-elements/smallqlistview.h 
	     ./sources/support/gui-elements/smallspinbox.h 
	     ./sources/support/gui-elements/super-frame.h 
	     ./sources/support/gui-elements/verysmallpushbutton.h 
	     ./sources/support/scan-handler.h 
	     ./sources/support/scantable-handler.h 
	     ./sources/support/audiosystem-selector.h
	     ./sources/support/dxDisplay.h
	     ./sources/support/converter_48000.h
	     ./sources/support/preset-handler.h
	     ./sources/support/presetcombobox.h
	     ./sources/support/scanlist-handler.h
	     ./sources/support/scheduler.h
	     ./sources/support/schedule-selector.h
	     ./sources/support/element-selector.h
	     ./sources/support/time-table.h
	     ./sources/support/content-table.h
	     ./sources/support/coordinates.h
	     ./sources/support/skin-handler.h
	     ./sources/support/mapport.h
	     ./sources/support/http-handler.h
	     ./sources/backend/msc-handler.h
	     ./sources/backend/backend.h
	     ./sources/backend/audio/mp2processor.h
	     ./sources/backend/audio/mp4processor.h
	     ./sources/backend/data/data-processor.h
	     ./sources/backend/data/virtual-datahandler.h
	     ./sources/backend/data/pad-handler.h
	     ./sources/backend/data/mot/mot-handler.h
	     ./sources/backend/data/mot/mot-object.h
	     ./sources/backend/data/mot/mot-dir.h
	     ./sources/backend/data/ip-datahandler.h
	     ./sources/backend/data/adv-datahandler.h
	     ./sources/backend/data/tdc-datahandler.h
	     ./sources/backend/data/journaline-datahandler.h
	     ./sources/backend/data/journaline-screen.h
	     ./sources/backend/data/epg/epg-compiler.h
	     ./sources/backend/data/epg/xml-extractor.h
	     ./sources/output/audio-player.h
	)

	set (${objectName}_UIS
	        ${${objectName}_UIS}
	     ./sources/main/aboutdialog.ui
	     ./sources/main/forms-v7/dabradio-6.ui
	     ./sources/main/forms-v7/technical_data.ui
	     ./sources/main/forms-v7/config-helper.ui
	     ./sources/main/forms-v7/new-display/scopewidget.ui
	     ./sources/main/forms-v7/snr-viewer/snr-widget.ui
	     ./sources/devices/filereaders/xml-filereader/xmlfiles.ui
	   )


#########################################################################

	if (FDK_AAC)
	   find_package(Fdk-AAC)
	   if (NOT LIBFDK_AAC_FOUND)
	      message(FATAL_ERROR, "Please install libfdk-aac")
	   endif ()
	   include_directories (../helpers/specials/fdk-aac)
	   set($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./sources//backend/audio/fdk-aac.h
	   )

	   set(${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./sources/backend/audio/fdk-aac.cpp
	   )
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./sources/backend/audio/fdk-aac.h
	   )
	   list(APPEND extraLibs ${FDK_AAC_LIBRARIES})
	   add_definitions(-D__WITH_FDK_AAC__)
	elseif (NOT FDK_AAC)
	   find_package (Faad)
	   if (NOT FAAD_FOUND)
	      message(FATAL_ERROR "Please install libfaad")
	   endif ()
	   set(${objectName}_HDRS
	       ${${objectName}_HDRS}
	       ./sources/backend/audio/faad-decoder.h
	   )

	   set(${objectName}_SRCS
	       ${${objectName}_SRCS}
	       ./sources/backend/audio/faad-decoder.cpp
	   )
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./sources/backend/audio/faad-decoder.h
	   )
	   add_definitions(-D__WITH_FAAD__)
	endif (FDK_AAC)

#	the viterbi decoder
#
	if (VITERBI_NEON)
	   set(VITERBI_SSE false)
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./sources/support/viterbi-spiral/spiral-neon.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./sources/support/viterbi-spiral/spiral-neon.c
	   )
	   
	   add_definitions (-DNEON_AVAILABLE)
	elseif(VITERBI_SSE)
	   include_directories (
	      ./sources/support/viterbi-spiral
	      ./sources/support/viterbi-spiral/includes
	   )
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./sources.support/viterbi-spiral/viterbi.h
	        ./sources/support/viterbi-spiral/spiral-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./sources/support/viterbi-spiral/viterbi.cpp
	        ./sources/support/viterbi-spiral/spiral-sse.c
	   )
	   add_definitions (-DSSE_AVAILABLE)
	else (VITERBI_SSE)
	   include_directories (
	      ./sources/support/viterbi-spiral
	   )
	   set ($(objectName)_HDRS
	        ${${objectName}_HDRS}
	        ./sources/support/viterbi-spiral/viterbi.h
	        ./sources/support/viterbi-spiral/spiral-no-sse.h
	   )
	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./sources/support/viterbi-spiral/viterbi.cpp
	        ./sources/support/viterbi-spiral/spiral-no-sse.c
	   )
	endif (VITERBI_NEON)

	if (TCP_STREAMER)
	   add_definitions (-DTCP_STREAMER)
	   find_package (Qt6Network REQUIRED)
	   set (${objectName}_MOCS
	        ${${objectName}_MOCS}
	        ./sources/output/tcp-streamer.h
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	        ./sources/output/tcp-streamer.h
	   )

	   set (${objectName}_SRCS
	        ${${objectName}_SRCS}
	        ./sources/output/tcp-streamer.cpp
	   )
	endif (TCP_STREAMER)

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

	if (NOT TCP_STREAMER)
	   add_definitions (-DQT_AUDIO)
           find_package(Portaudio)
           if (NOT PORTAUDIO_FOUND)
               message(FATAL_ERROR "please install portaudio V19")
           endif ()
           list(APPEND extraLibs ${PORTAUDIO_LIBRARIES})
	   find_package (Qt6Multimedia REQUIRED)
	   include_directories (
	      ${Qt6Multimedia_INCLUDE_DIRS}
	      ./sources/output/portaudio
	      ./sources/output/Qt6
	   )
	   set (${objectName}_HDRS
	        ${${objectName}_HDRS}
	       ./sources/output/portadio/audiosink.h
	       ./sources/output/Qt6/Qt-audio.h
	       ./sources/output/Qt6/Qt-audiodevice.h
	   )

	   set (${objectName}_MOCS
	       ${${objectName}_MOCS}
	       ./sources/output/portaudio/audiosink.h
	       ./sources/output/Qt6/Qt-audio.h
	       ./sources/output/Qt6/Qt-audiodevice.h
	   )

	   set (${objectName}_SRCS
	      ${${objectName}_SRCS}
	      ./sources/output/portaudio/audiosink.cpp
	      ./sources/output/Qt6/Qt-audio.cpp
	      ./sources/output/Qt6/Qt-audiodevice.cpp
	   )
	endif (NOT TCP_STREAMER)

	QT6_WRAP_UI (UIS ${${objectName}_UIS}
	             ./sources/devices/filereaders/filereader-widget.ui)

	include_directories (
	          ${SDRPLAY_INCLUDES}
	          ${QT6Widgets_INCLUDES}
	          ${QT_QTCORE_INCLUDE_DIR}
	          ${QT_QTGUI_INCLUDE_DIR}
	          ${PORTAUDIO_INCLUDE_DIRS}
	          ${FAAD_INCLUDE_DIRS}
	)

	QT6_WRAP_CPP (MOCS ${${objectName}_MOCS})

	add_executable (${objectName}
	                ${${objectName}_SRCS}
	                ${UIS}
	                ${RSCS}
	                ${TRS}
	                ${MOCS}
	                resources.qrc
	)

	target_link_libraries (${objectName}
	                       PRIVATE
	                       Qt6::Widgets
	                       Qt6::Network
	                       Qt6::Xml
	                       Qt6::Multimedia
	                       ${RTLTCP_lib}
	                       ${extraLibs}
	                       ${FAAD_LIBRARIES}
	                       ${CMAKE_DL_LIBS}
	)

	add_subdirectory (./sources/devices/ /tmp)
	target_link_libraries (${objectName} PRIVATE ${objectName}_devices)

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
