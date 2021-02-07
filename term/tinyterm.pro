TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

unix:LIBS += -lpthread

SOURCES += \
        main.cpp \
        serial.cpp

HEADERS += \
	serial.h
