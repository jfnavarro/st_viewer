TEMPLATE=lib
CONFIG += qt dll qtcolorpicker-buildlib
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
include(../src/qtcolorpicker.pri)
TARGET = $$QTCOLORPICKER_LIBNAME
DESTDIR = $$QTCOLORPICKER_LIBDIR
win32 {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${QTCOLORPICKER_LIBNAME}.dll
}
target.path = $$DESTDIR
INSTALLS += target
