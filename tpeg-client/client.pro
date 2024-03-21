#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network  widgets

INCLUDEPATH += . 

HEADERS     = ./client.h \
	      ./constants.h 

SOURCES		=  ./client.cpp main.cpp
TARGET		= Client
FORMS		+= ./widget.ui

unix{
DESTDIR     = .
}

