include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/libqxt)

set(LIBRARY_ARG_INCLUDES
    CellViewPage.h
    DatasetPage.h
    UserSelectionsPage.h
    ScrollArea.h
    GenesWidget.h
    SelectionsWidget.h
)

set(LIBRARY_ARG_SOURCES
    CellViewPage.cpp
    DatasetPage.cpp
    UserSelectionsPage.cpp
    ScrollArea.cpp
    GenesWidget.cpp
    SelectionsWidget.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectionsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellviewPage.ui"
)

ST_LIBRARY()

