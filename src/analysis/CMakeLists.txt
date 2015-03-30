include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  AnalysisDEA.h
  AnalysisFRD.h
)

set(LIBRARY_ARG_SOURCES
  AnalysisDEA.cpp
  AnalysisFRD.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/ddaWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/frdWidget.ui"
)
ST_LIBRARY()
