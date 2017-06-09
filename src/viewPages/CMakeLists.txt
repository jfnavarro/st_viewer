include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    CellViewPage.h
    DatasetPage.h
    UserSelectionsPage.h
    GenesWidget.h
    SpotsWidget.h
    SelectionGenesWidget.h
    SelectionSpotsWidget.h
    SettingsWidget.h
)

set(LIBRARY_ARG_SOURCES
    CellViewPage.cpp
    DatasetPage.cpp
    UserSelectionsPage.cpp
    GenesWidget.cpp
    SpotsWidget.cpp
    SelectionGenesWidget.cpp
    SelectionSpotsWidget.cpp
    SettingsWidget.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/datasetsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectionsPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/cellviewPage.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/SettingsWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/genesSelectionWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/spotsSelectionWidget.ui"
)

ST_LIBRARY()

