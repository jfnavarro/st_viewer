include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    DatasetImporter.h
)

set(LIBRARY_ARG_SOURCES
    DatasetImporter.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetImporter.ui"
)

ST_LIBRARY()

