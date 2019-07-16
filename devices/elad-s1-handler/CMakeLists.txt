cmake_minimum_required( VERSION 2.6 )
#set ( CMAKE_BUILD_TYPE Release )
set (objectName device_elad-s1)
ADD_DEFINITIONS (-DQT_SHARED)
ADD_DEFINITIONS (-DQT_NO_DEBUG)
set (CMAKE_INSTALL_PREFIX ../../../../../../../)
add_definitions ( -Wall )

	find_package (Qt5Widgets REQUIRED)
	find_package (Qt5Declarative REQUIRED)

	find_library (DYNAMIC_LOAD dl)
	if(NOT(DYNAMIC_LOAD))
	   message(FATAL_ERROR "please install -ldl")
	else(NOT(DYNAMIC_LOAD))
	   set(extraLibs ${DYNAMIC_LOAD})
	endif(NOT(DYNAMIC_LOAD))
#
	find_library (PTHREADS pthread)
	if(NOT(PTHREADS))
	   message (FATAL_ERROR "please install libpthread")
	else(NOT(PTHREADS))
	   set (extraLibs ${extraLibs} ${PTHREADS})
	endif(NOT(PTHREADS))

	set (${objectName}_UIS ./widget.ui)

	include_directories (
		${CMAKE_SOURCE_DIR}
	        ${CMAKE_CURRENT_BINARY_DIR}
	        ${QT5Widgets_INCLUDES}
		${QT_QTCORE_INCLUDE_DIR}
	        ${QT_QTGUI_INCLUDE_DIR}
		.
		../
		../.. 
		../../radio-utils 
		../../../includes 
		../../../includes/filters 
		../../../includes/various 
		../../../includes/output
	)

	set ( ${objectName}_HDRS
		../rig-interface.h 
		./elad-s1.h 
		./elad-worker.h 
		./elad-loader.h 
		../../../includes/swradio-constants.h 
		../../../includes/filters/fir-filters.h 
		../../../includes/various/ringbuffer.h
	)

	set ( ${objectName}_SRCS
		./elad-s1.cpp
		./elad-worker.cpp 
		./elad-loader.cpp 
		../../../src/filters/fir-filters.cpp
		../../../src/filters/iir-filters.cpp 
		../../../src/various/sincos.cpp 
		../../../src/various/utilities.cpp 
	)

	set (${objectName}_UIS ./widget.ui)
	QT5_WRAP_UI(UIS ${${objectName}_UIS})

	set ( ${objectName}_MOCS
		../rig-interface.h 
		./elad-s1.h
	)

	add_library (${objectName}
	             SHARED ${${objectName}_SRCS}
	             ${UIS} ${RSCS} ${TRS} ${MOCS}
	)
	 
	target_link_libraries (${objectName}
	               Qt5::Widgets
	               ${extraLibs}
	               ${SAMPLERATE_LIBRARIES}
	)

	INSTALL(TARGETS ${objectName} LIBRARY DESTINATION ./linux-bin/input-plugins-sw)
