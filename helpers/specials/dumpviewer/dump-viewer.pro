#
TEMPLATE    = app
CONFIG      += console
QT          += widgets

INCLUDEPATH += . \
	      /usr/include/qt5/qwt

HEADERS     = ./dump-viewer.h 
SOURCES     = ./dump-viewer.cpp main.cpp
TARGET      = dumpViewer
FORMS		+= ./dumpwidget.ui

unix{
DESTDIR     = ./linux-bin
LIBS		+= -lqwt-qt5
}

