FIX_QT3D_COMPILATION_BUG()

set(LIBRARY_ARG_INCLUDES
    ColorPalette.h
    HeatMap.h
    ColorListEditor.h
)

set(LIBRARY_ARG_SOURCES
    ColorPalette.cpp
    HeatMap.cpp
    ColorListEditor.cpp
)
set(LIBRARY_ARG_UI_FILES)
ST_LIBRARY()

