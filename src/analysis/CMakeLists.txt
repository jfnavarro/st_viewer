include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  AnalysisDEA.h
  AnalysisFRD.h
  AnalysisCorrelation.h
)

set(LIBRARY_ARG_SOURCES
  AnalysisDEA.cpp
  AnalysisFRD.cpp
  AnalysisCorrelation.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/ddaWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/frdWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/AnalysisCorrelation.ui"
)
ST_LIBRARY()
