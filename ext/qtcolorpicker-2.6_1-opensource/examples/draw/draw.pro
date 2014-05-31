TEMPLATE = app
INCLUDEPATH += .
QT += widgets
include(../../src/qtcolorpicker.pri)

# Input
HEADERS += draw.h drawboard.h
SOURCES += main.cpp draw.cpp drawboard.cpp
