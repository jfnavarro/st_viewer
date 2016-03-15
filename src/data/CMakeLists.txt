include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    DataProxy.h
    ObjectParser.h
    DatasetImporter.h
)

set(LIBRARY_ARG_SOURCES
    DataProxy.cpp
    ObjectParser.cpp
    DatasetImporter.cpp
)

set(LIBRARY_ARG_UI_FILES
    datasetImporter.ui
)

ST_LIBRARY()

