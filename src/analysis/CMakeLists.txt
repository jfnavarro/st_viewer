include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  AnalysisDEA.h
  AnalysisFRD.h
  AnalysisCorrelation.h
  AnalysisClustering.h
)

set(LIBRARY_ARG_SOURCES
  AnalysisDEA.cpp
  AnalysisFRD.cpp
  AnalysisCorrelation.cpp
  AnalysisClustering.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/ddaWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/frdWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/AnalysisCorrelation.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/analysisclustering.ui"
)
ST_LIBRARY()
