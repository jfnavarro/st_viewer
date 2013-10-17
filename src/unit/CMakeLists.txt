###############################################################################
# Unit Test CMake                                                             #
###############################################################################

use_qt5lib(Qt5Test)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${CMAKE_BINARY_DIR}/src ${CMAKE_BINARY_DIR})

set(TARGET_NAME stVi_test)

set(ST_MAIN
    controller/data/tst_datastoretest.cpp
    controller/data/tst_datastoretest.h
    controller/tst_widgets.cpp
    controller/tst_widgets.h
	model/tst_objectparsertest.cpp
	model/tst_objectparsertest.h
    utils/tst_mathextendedtest.cpp
    utils/tst_mathextendedtest.h
)

set(LIB_ST_CORE
    libs/STCore/tst_simplecryptdevicetest.cpp
    libs/STCore/tst_simplecryptdevicetest.h
    libs/STCore/tst_simplecrypttest.cpp
    libs/STCore/tst_simplecrypttest.h
)

set(LIB_ST_CORE_WIDGETS 
)

set(LIB_ST_NETWORK
    network/test_auth.cpp
    network/test_rest.cpp
)

set(LIB_ST_OPENGL
    libs/STOpenGL/compression/tst_glcolortest.cpp
    libs/STOpenGL/compression/tst_glcolortest.h
    libs/STOpenGL/math/tst_glaabbtest.cpp
    libs/STOpenGL/math/tst_glaabbtest.h
    libs/STOpenGL/math/tst_glfloattest.cpp
    libs/STOpenGL/math/tst_glfloattest.h
    libs/STOpenGL/math/tst_glmathtest.cpp
    libs/STOpenGL/math/tst_glmathtest.h
    libs/STOpenGL/math/tst_glmatrixtest.cpp
    libs/STOpenGL/math/tst_glmatrixtest.h
    libs/STOpenGL/math/tst_glquadtreetest.cpp
    libs/STOpenGL/math/tst_glquadtreetest.h
    libs/STOpenGL/math/tst_glvectortest.cpp
    libs/STOpenGL/math/tst_glvectortest.h
    libs/STOpenGL/render/tst_glheatmaptest.cpp
    libs/STOpenGL/render/tst_glheatmaptest.h
    libs/STOpenGL/tst_glcommontest.cpp
    libs/STOpenGL/tst_glcommontest.h
    libs/STOpenGL/tst_glpointtest.cpp
    libs/STOpenGL/tst_glpointtest.h
    libs/STOpenGL/tst_gltypetraitstest.cpp
    libs/STOpenGL/tst_gltypetraitstest.h
)

# Define source files
set(SRC_${TARGET_NAME}
    ${ST_MAIN}
    ${LIB_ST_CORE}
    ${LIB_ST_CORE_WIDGETS}
    ${LIB_ST_OPENGL}
    ${LIB_ST_NETWORK}
)

# add executable
if(WIN32)
    add_executable(${TARGET_NAME} WIN32 unit_main.cpp ${SRC_${TARGET_NAME}})
    target_link_libraries(${TARGET_NAME} ${Qt5Core_QTMAIN_LIBRARIES}) #Note: allows compile windows application (ie. not console)
else()
    add_executable(${TARGET_NAME} unit_main.cpp ${SRC_${TARGET_NAME}})
endif()

target_link_libraries(${TARGET_NAME} STMain QtDebugHelper QtUnitTest STOpenGL STCore STCoreWidgets)
qt5_use_modules(${TARGET_NAME} Test)

add_test(general ${TARGET_NAME})
add_custom_target(test ${TARGET_NAME})
