# Files that make up OpenGL

set(LIB STOpenGL)

message(STATUS "Building ${LIB}")

set(SRC_${LIB}

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/compression/GLBitColor.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementData.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementDataGene.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementRender.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementShapeFactory.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLElementShapeFactory.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTexture.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTextureCutter.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTextureCutter.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTextureData.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTextureRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/data/GLTextureRender.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/image/GLHeatMap.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/image/GLHeatMap.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/image/GLImage.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/image/GLImageWriter.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/image/GLImageWriter.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLAABB.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLAlgorithm.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLFloat.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLMath.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLMatrix.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLQuadTree.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLQuadTree.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/math/GLVector.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/shader/GLShader.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/shader/GLShaderProgram.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/shader/GLShaderRender.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/shader/GLShaderRender.h

    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLColor.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLColor.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLCommon.cpp
    #${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLCommon.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLInplace.h
    ${CMAKE_SOURCE_DIR}/src/libs/${LIB}/src/GLQt.h
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
