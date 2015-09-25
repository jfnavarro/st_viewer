include_directories(${PROJECT_SOURCE_DIR}/ext;${PROJECT_SOURCE_DIR}/ext/qjpeg-turbo)

set(LIBRARY_ARG_INCLUDES
    AssertOpenGL.h
    GeneRendererGL.h
    GeneData.h
    GridRendererGL.h
    CellGLView.h
    HeatMapLegendGL.h
    ImageTextureGL.h
    MiniMapGL.h
    Renderer.h
    ColoredQuads.h
    ColoredLines.h
    STTexturedQuads.h
    GraphicItemGL.h
    SelectionEvent.h
    RubberbandGL.h
)

set(LIBRARY_ARG_SOURCES
    AssertOpenGL.cpp
    GeneRendererGL.cpp
    GridRendererGL.cpp
    GeneData.cpp
    CellGLView.cpp
    HeatMapLegendGL.cpp
    ImageTextureGL.cpp
    MiniMapGL.cpp
    Renderer.cpp
    ColoredQuads.cpp
    ColoredLines.cpp
    STTexturedQuads.cpp
    GraphicItemGL.cpp
    RubberbandGL.cpp
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
