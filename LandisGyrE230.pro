#-------------------------------------------------
#
# Project created by QtCreator 2021-05-25T13:49:15
#
#-------------------------------------------------
QT       += core
QT       -= gui

CONFIG += c++11

TEMPLATE = lib
DEFINES += LANDISGYRE230_LIBRARY

CONFIG += c++11


CONFIG         += plugin

DEFINES += LANDISGYRE230_LIBRARY
DEFINES += STANDARD_METER_PLUGIN
DEFINES += METERPLUGIN_FILETREE


include(../../../Matilda-units/meter-plugin-shared/meter-plugin-shared/meter-plugin-shared.pri)
include(../../../Matilda-units/matilda-base/type-converter/type-converter.pri)
include(../../../Matilda-units/meter-plugin-shared/IEC62056-21-Processor/IEC62056-21-Processor.pri)



EXAMPLE_FILES = zbyralko.json

linux-beagleboard-g++:{
    target.path = /opt/matilda/plugin
    INSTALLS += target
}

linux-g++{
target.path = /opt/plugin2/
INSTALLS += target
}

HEADERS += \
    landisgyre230.h \
    landisgyre230encoderdecoder.h

SOURCES += \
    landisgyre230.cpp \
    landisgyre230encoderdecoder.cpp
