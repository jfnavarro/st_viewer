###############################################################################
# Unit Test CMake                                                             #
###############################################################################

use_qt5lib(Qt5Test)

include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}/src/testTools
                    ${PROJECT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}/src
                    ${CMAKE_BINARY_DIR})

# Define source files
set(ST_UNITTEST_SOURCES
    ${ST_MAIN}
    ${LIB_ST_SIMPLECRYPT}
    ${LIB_ST_MATH}
    ${LIB_ST_NETWORK}
    $<TARGET_OBJECTS:testTools>
    ${ST_TARGET_OBJECTS}
)

find_package(Qt5Test REQUIRED)

macro(add_st_client_test subdir name)
  set(srcs ${ST_UNITTEST_SOURCES} ${subdir}/${name}.cpp ${subdir}/${name}.h)
  if(WIN32)
    add_executable(${name} WIN32 ${srcs})
    target_link_libraries(${name} Qt5::WinMain) #Note: allows compile windows application (ie. not console)
  else()
    add_executable(${name} ${srcs})
  endif()
  target_link_libraries(${name} ${QT_TARGET_LINK_LIBS} ${OPENGL_LIBRARY} ${LIBJPEG_LIBRARIES} Qt5::Test)
  add_test(NAME ${name}
           COMMAND $<TARGET_FILE:${name}>)
  add_dependencies(${name} testTools)
endmacro()

add_st_client_test(controller tst_widgets)
add_st_client_test(model tst_objectparsertest)
add_st_client_test(utils tst_mathextendedtest)
add_st_client_test(network test_auth)
add_st_client_test(network test_rest)
add_st_client_test(math tst_glaabbtest)
add_st_client_test(math tst_glquadtreetest)
add_st_client_test(math tst_glheatmaptest)
