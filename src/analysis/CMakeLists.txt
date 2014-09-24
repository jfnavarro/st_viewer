FIX_QT3D_COMPILATION_BUG()

include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  AnalysisDEA.h
  AnalysisHistogram.h
)

set(LIBRARY_ARG_SOURCES
  AnalysisDEA.cpp
  AnalysisHistogram.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/ddaWidget.ui"
    "${PROJECT_SOURCE_DIR}/assets/ui/readsHistogramWidget.ui"
)
ST_LIBRARY()
