include_directories(${PROJECT_SOURCE_DIR}/ext)

set(LIBRARY_ARG_INCLUDES
    GeneRendererGL.h
    GeneData.h
    GridRendererGL.h
    CellGLView.h
    HeatMapLegendGL.h
    ImageTextureGL.h
    GraphicItemGL.h
    SelectionEvent.h
    RubberbandGL.h
)

set(LIBRARY_ARG_SOURCES
    GeneRendererGL.cpp
    GridRendererGL.cpp
    GeneData.cpp
    CellGLView.cpp
    HeatMapLegendGL.cpp
    ImageTextureGL.cpp
    GraphicItemGL.cpp
    RubberbandGL.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
