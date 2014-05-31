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
    SpinBoxSlider.h
    ScrollArea.h
)

set(LIBRARY_ARG_SOURCES
    CellViewPage.cpp
    DatasetPage.cpp
    ExperimentPage.cpp
    InitPage.cpp
    Page.cpp
    CellViewPageToolBar.cpp
    ExtendedTabWidget.cpp
    SpinBoxSlider.cpp
    ScrollArea.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasets.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/experiments.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/initpage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellview.ui"
)

ST_LIBRARY()

