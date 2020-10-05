TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ModdedFloodFill/*.cpp \
    ModdedFloodFill/Map.cpp \
    ModdedFloodFill/Test.cpp

HEADERS += ModdedFloodFill/*.h \
    ModdedFloodFill/Map.h \
    ModdedFloodFill/Test.h
