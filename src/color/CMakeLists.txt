FIX_QT3D_COMPILATION_BUG()

include_directories(${PROJECT_SOURCE_DIR}/ext/qtcolorpicker-2.6_1-opensource/src)

set(LIBRARY_ARG_INCLUDES
    ColorPalette.h
    HeatMap.h
)

set(LIBRARY_ARG_SOURCES
    ColorPalette.cpp
    HeatMap.cpp
)
set(LIBRARY_ARG_UI_FILES)
ST_LIBRARY()

