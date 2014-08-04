include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/libqxt)

set(LIBRARY_ARG_INCLUDES
    SpinBoxSlider.h
)

set(LIBRARY_ARG_SOURCES
    SpinBoxSlider.cpp
)

set(LIBRARY_ARG_UI_FILES)

ST_LIBRARY()

