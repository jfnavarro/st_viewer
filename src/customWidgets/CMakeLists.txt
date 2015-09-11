# We want to use QXT as a static lib. By default its header file assumes dynamic linkage.
add_definitions(-DQXT_STATIC)

include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/libqxt)

set(LIBRARY_ARG_INCLUDES
    SpinBoxSlider.h
)

set(LIBRARY_ARG_SOURCES
    SpinBoxSlider.cpp
)

set(LIBRARY_ARG_UI_FILES)

ST_LIBRARY()

