include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/libqxt)

set(LIBRARY_ARG_INCLUDES
    CellViewPage.h
    DatasetPage.h
    ExperimentPage.h
    InitPage.h
    Page.h
    AnimatedDatasetsTableView.h
    GenesTableView.h
    GeneSelectionTableView.h
    WidgetBackgroundAnimation.h
    CellViewPageToolBar.h
    BooleanItemDelegate.h
    DatasetsViewItemDelegate.h
    GeneViewDelegate.h
    ColorListEditor.h
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
    AnimatedDatasetsTableView.cpp
    GenesTableView.cpp
    GeneSelectionTableView.cpp
    WidgetBackgroundAnimation.cpp
    CellViewPageToolBar.cpp
    BooleanItemDelegate.cpp
    DatasetsViewItemDelegate.cpp
    GeneViewDelegate.cpp
    ColorListEditor.cpp
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

