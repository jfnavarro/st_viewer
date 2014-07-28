include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qcustomplot)

set(LIBRARY_ARG_INCLUDES
  ScatterPlot.h
  AnalysisDEA.h
)

set(LIBRARY_ARG_SOURCES
  ScatterPlot.cpp
  AnalysisDEA.cpp
)

set(LIBRARY_ARG_UI_FILES
    "${PROJECT_SOURCE_DIR}/assets/ui/plotCanvas.ui"
)
ST_LIBRARY()
