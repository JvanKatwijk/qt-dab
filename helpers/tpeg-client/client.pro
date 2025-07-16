#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network  widgets

QMAKE_CFLAGS	+=  -g -fsanitize=address 
QMAKE_CXXFLAGS	+=  -g -fsanitize=address 
QMAKE_LFLAGS	+=  -g -fsanitize=address

INCLUDEPATH += . \
	/usr/include/python3.11/

HEADERS     = ./client.h \
	      ./constants.h 

SOURCES		=  ./client.cpp main.cpp 
TARGET		= Client
FORMS		+= ./widget.ui
LIBS		+=-lpython3.11 -ldl -lm
unix{
DESTDIR     = .
}

