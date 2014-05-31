include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(simple main.cpp)
target_link_libraries(simple ${QT_TARGET_LINK_LIBRARIES} ${lib_name})
