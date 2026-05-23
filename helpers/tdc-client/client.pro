#
TEMPLATE    = app
CONFIG      += console
QT          += core gui network  widgets

QMAKE_CFLAGS	+=  -g -fsanitize=address 
QMAKE_CXXFLAGS	+=  -g -fsanitize=address 
QMAKE_LFLAGS	+=  -g -fsanitize=address

HEADERS		= ./tdc-client.h
SOURCES		=  ./tdc-client.cpp
TARGET		= Client
unix{
DESTDIR     = .
}

