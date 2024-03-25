#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network  widgets

INCLUDEPATH += . \
	      cpp-base64
	

HEADERS     = ./client.h \
	      ./constants.h \
	      ./cpp-base64/base64.h

SOURCES		=  ./client.cpp main.cpp ./cpp-base64/base64.cpp
TARGET		= Client
FORMS		+= ./widget.ui

unix{
DESTDIR     = .
}

