include_directories(${LIBJPEG_INCLUDE_DIRS})

set(LIBRARY_ARG_INCLUDES
    Utils.h
    SetTips.h
    SerializationFunctions.h
    QJpegImageReader.h
)

set(LIBRARY_ARG_SOURCES
    SetTips.cpp
    QJpegImageReader.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
