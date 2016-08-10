###############################################################################
# Unit Test CMake                                                             #
###############################################################################

use_qt5lib(Qt5Test)

include_directories(${PROJECT_SOURCE_DIR}/src
                    ${PROJECT_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}/src
                    ${CMAKE_BINARY_DIR})

# Define source files
set(ST_UNITTEST_SOURCES
    ${ST_MAIN}
    ${LIB_ST_SIMPLECRYPT}
    ${LIB_ST_MATH}
    ${LIB_ST_NETWORK}
    ${ST_TARGET_OBJECTS}
)

find_package(Qt5Test REQUIRED)

### TEST CREATION MACRO #######################################################
# This macro accepts an optional argument 'otherfiles'. This forms an optional list of non test 
# files to be added to test executable. It assumes that each entry 'foo' in the list has a 
# corresponding .h and .cpp file located in the named sub directory.
macro(add_st_client_test subdir name)
  set(srcs ${ST_UNITTEST_SOURCES} ${subdir}/${name}.h ${subdir}/${name}.cpp )
  set (otherfiles ${ARGN})
  foreach(file ${otherfiles})
    set(srcs ${srcs} ${subdir}/${file}.h ${subdir}/${file}.cpp )
  endforeach()
  add_executable(${name} ${srcs})
  target_link_libraries(${name} ${QT_TARGET_LINK_LIBS} qcustomplot Qt5::Test)
  add_test(NAME ${name}
           COMMAND $<TARGET_FILE:${name}>)

  add_dependencies(${name} ${PROJECT_NAME})

  if(WIN32)
      string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWERCASE)
      if(BUILD_TYPE_LOWERCASE STREQUAL "debug")
          get_target_property(ST_QT_LOC "Qt5::Test" LOCATION_DEBUG)
      else()
          get_target_property(ST_QT_LOC "Qt5::Test" LOCATION)
      endif()
      install(FILES ${ST_QT_LOC} DESTINATION .)
      add_custom_command(TARGET ${name} POST_BUILD  
                         COMMAND ${CMAKE_COMMAND} -E copy ${ST_QT_LOC}
                         ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/)
  endif(WIN32)
endmacro()


### ST UNIT TESTS LIST ########################################################
add_st_client_test(controller tst_widgets)
add_st_client_test(model tst_objectparsertest)
add_st_client_test(utils tst_mathextendedtest)
add_st_client_test(network test_auth)
add_st_client_test(network test_rest)
add_st_client_test(math tst_glaabbtest)
add_st_client_test(math tst_glquadtreetest)
add_st_client_test(math tst_glheatmaptest)
