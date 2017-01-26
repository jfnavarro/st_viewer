include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    DataProxy.h
    DatasetImporter.h
)

set(LIBRARY_ARG_SOURCES
    DataProxy.cpp
    DatasetImporter.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetImporter.ui"
)

ST_LIBRARY()

