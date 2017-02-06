include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    DatasetImporter.h
    Dataset.h
    Spot.h
    Gene.h
    UserSelection.h
    STData.h
)

set(LIBRARY_ARG_SOURCES
    DatasetImporter.cpp
    Dataset.cpp
    Spot.cpp
    Gene.cpp
    UserSelection.cpp
    STData.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetImporter.ui"
)

ST_LIBRARY()

