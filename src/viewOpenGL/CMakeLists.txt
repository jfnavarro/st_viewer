include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qjpeg-turbo)

set(LIBRARY_ARG_INCLUDES
    GeneRendererGL.h
    GeneData.h
    GridRendererGL.h
    CellGLView.h
    HeatMapLegendGL.h
    ImageTextureGL.h
    MiniMapGL.h
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
    MiniMapGL.cpp
    GraphicItemGL.cpp
    RubberbandGL.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
