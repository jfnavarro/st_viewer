include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  AnalysisDEA.h
  AnalysisQC.h
  AnalysisCorrelation.h
  AnalysisClustering.h
  ChartView.h
)

set(LIBRARY_ARG_SOURCES
  AnalysisDEA.cpp
  AnalysisQC.cpp
  AnalysisCorrelation.cpp
  AnalysisClustering.cpp
  ChartView.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/analysisDEA.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/analysisQC.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/analysisCorrelation.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/analysisClustering.ui"
)
ST_LIBRARY()
