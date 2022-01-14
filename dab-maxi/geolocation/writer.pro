
TEMPLATE	= app
TARGET		= coord-writer
QMAKE_CXXFLAGS	+= -std=c++11
QMAKE_CFLAGS	+=  -flto -ffast-math
MAKE_CXXFLAGS	+=  -flto -ffast-math
#QMAKE_CFLAGS	+=  -g
#QMAKE_CXXFLAGS	+=  -g
#QMAKE_LFLAGS	+=  -g
QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]

DEPENDPATH += . 
INCLUDEPATH += . 
# Input
SOURCES += ./writer.cpp 
