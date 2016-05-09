include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    CellViewPage.h
    DatasetPage.h
    UserSelectionsPage.h
    GenesWidget.h
    SelectionsWidget.h
)

set(LIBRARY_ARG_SOURCES
    CellViewPage.cpp
    DatasetPage.cpp
    UserSelectionsPage.cpp
    GenesWidget.cpp
    SelectionsWidget.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectionsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellviewPage.ui"
)

ST_LIBRARY()

