include_directories(${PROJECT_SOURCE_DIR}/ext/qtcolorpicker-2.6_1-opensource/src)

set(LIBRARY_ARG_INCLUDES
    DatasetsTableView.h
    GenesTableView.h
    GeneSelectionTableView.h
    BooleanItemDelegate.h
    GeneViewDelegate.h
    ExperimentsTableView.h
)

set(LIBRARY_ARG_SOURCES
    DatasetsTableView.cpp
    GenesTableView.cpp
    GeneSelectionTableView.cpp
    BooleanItemDelegate.cpp
    GeneViewDelegate.cpp
    ExperimentsTableView.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()

