TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += ModdedFloodFill/*.cpp \
           AStar_DFS/*.cpp

HEADERS += ModdedFloodFill/*.h \
           AStar_DFS/*.h
