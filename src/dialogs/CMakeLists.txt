set(LIBRARY_ARG_INCLUDES
    AboutDialog.h
    SelectionDialog.h
    LoginDialog.h
    EditSelectionDialog.h
    EditDatasetDialog.h
)

set(LIBRARY_ARG_SOURCES
    AboutDialog.cpp
    SelectionDialog.cpp
    LoginDialog.cpp
    EditSelectionDialog.cpp
    EditDatasetDialog.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/aboutdialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectionConsole.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/login.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/editSelectionDialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/editDatasetDialog.ui"
)

ST_LIBRARY()
