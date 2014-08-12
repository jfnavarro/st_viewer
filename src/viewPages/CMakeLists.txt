include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/libqxt)

FIX_QT3D_COMPILATION_BUG()

set(LIBRARY_ARG_INCLUDES
    CellViewPage.h
    DatasetPage.h
    ExperimentPage.h
    InitPage.h
    Page.h
    CellViewPageToolBar.h
    ExtendedTabWidget.cpp
    ScrollArea.h
    GenesWidget.h
    SelectionsWidget.h
)

set(LIBRARY_ARG_SOURCES
    CellViewPage.cpp
    DatasetPage.cpp
    ExperimentPage.cpp
    InitPage.cpp
    Page.cpp
    CellViewPageToolBar.cpp
    ExtendedTabWidget.cpp
    ScrollArea.cpp
    GenesWidget.cpp
    SelectionsWidget.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasets.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/experiments.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/initpage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellview.ui"
)

ST_LIBRARY()

