



include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(draw main.cpp draw.cpp draw.h drawboard.cpp drawboard.h)
target_link_libraries(draw ${QT_TARGET_LINK_LIBRARIES} ${lib_name})
