set(LIBRARY_ARG_INCLUDES
    #GeneRendererGL.h (needs to be adapted to Qt3D)
    GridRendererGL.h
    CellGLView.h
    #HeatMapLegendGL.h (needs to be adapted to Qt3D)
    ImageTextureGL.h
    #MiniMapGL.h  (needs to be adapted to Qt3D)
    SelectionEvent.h
)

set(LIBRARY_ARG_SOURCES
    #GeneRendererGL.cpp (needs to be adapted to Qt3D)
    GridRendererGL.cpp
    CellGLView.cpp
    #HeatMapLegendGL.cpp (needs to be adapted to Qt3D)
    ImageTextureGL.cpp
    #MiniMapGL.cpp (needs to be adapted to Qt3D)
)

set(LIBRARY_ARG_UI_FILES
)

ST_LIBRARY()
