#
TEMPLATE    = lib
CONFIG      += plugin
QT          += core gui widgets

INCLUDEPATH += . \
	       ../ ../.. \
	       ../../../includes \
	       ../../../includes/various
HEADERS     = ../rig-interface.h \
	      ../../../includes/swradio-constants.h \
	      ./elad-s1.h \
	      ./elad-worker.h \
	      ./elad-loader.h \
	      ../../../includes/various/ringbuffer.h
SOURCES     =  ./elad-s1.cpp \
	       ./elad-worker.cpp \
	       ./elad-loader.cpp 
TARGET      = $$qtLibraryTarget(device_elad-s1sw)
FORMS	+= ./widget.ui

unix{
DESTDIR     = ../../../../../linux-bin/input-plugins-sw
LIBS	+= -lusb-1.0 -ldl
}

