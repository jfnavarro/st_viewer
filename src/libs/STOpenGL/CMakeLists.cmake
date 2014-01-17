# Files that make up OpenGL

set(LIB STOpenGL)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/compression/GLBitColor.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementData.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementData.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementDataGene.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementDataGene.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementShapeFactory.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementShapeFactory.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementLineFactory.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementLineFactory.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementRectangleFactory.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementRectangleFactory.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLHeatMap.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLHeatMap.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureCutter.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureCutter.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureData.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureData.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/texture/GLTextureRender.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLAABB.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLQuadTree.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/render/GLShaderRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/render/GLShaderRender.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/render/GLElementRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/render/GLElementRender.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLColor.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLColor.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLCommon.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLCommon.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLScope.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLScope.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLTypeTraits.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLTypeTraits.h
)

# Add library and include directory
add_library(${LIB} STATIC ${SRC_${LIB}})
target_link_libraries(${LIB} ${OPENGL_LIBRARY} )
include_directories(${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src)

# Organize lib into solution folder
project_group(${LIB} "libs")

# Clean-up
unset(SRC_${LIB})
unset(LIB)
