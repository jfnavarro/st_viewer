set(LIBRARY_ARG_INCLUDES
    AboutDialog.h
    SelectionDialog.h
    LoginDialog.h
)

set(LIBRARY_ARG_SOURCES
    AboutDialog.cpp
    SelectionDialog.cpp
    LoginDialog.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/aboutdialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/selectiondialog.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/login.ui"
)

ST_LIBRARY()
