set(LIBRARY_ARG_INCLUDES
    data/GLElementData.h
    data/GLElementDataGene.h
    data/GLElementShapeFactory.h
    data/GLElementLineFactory.h
    data/GLElementRectangleFactory.h
    data/GLHeatMap.h
    texture/GLTextureCutter.h
    texture/GLTextureData.h
    texture/GLTextureRender.h
    math/GLAABB.h
    math/GLQuadTree.h
    render/GLShaderRender.h
    render/GLElementRender.h
    GLCommon.h
    GLScope.h
    GLTypeTraits.h
)

set(LIBRARY_ARG_SOURCES
    data/GLElementData.cpp
    data/GLElementDataGene.cpp
    data/GLElementShapeFactory.cpp
    data/GLElementLineFactory.cpp
    data/GLElementRectangleFactory.cpp
    data/GLHeatMap.cpp
    texture/GLTextureCutter.cpp
    texture/GLTextureData.cpp
    texture/GLTextureRender.cpp
    render/GLShaderRender.cpp
    render/GLElementRender.cpp
    GLCommon.cpp
    GLScope.cpp
    GLTypeTraits.cpp
)

set(LIBRARY_ARG_UI_FILES
)

st_library()



