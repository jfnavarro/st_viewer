infile(config.pri, SOLUTIONS_LIBRARY, yes): CONFIG += qtcolorpicker-uselib
TEMPLATE += fakelib
QTCOLORPICKER_LIBNAME = $$qtLibraryTarget(QtSolutions_ColorPicker-2.6)
TEMPLATE -= fakelib
QTCOLORPICKER_LIBDIR = $$PWD/lib
unix:qtcolorpicker-uselib:!qtcolorpicker-buildlib:QMAKE_RPATHDIR += $$QTCOLORPICKER_LIBDIR
