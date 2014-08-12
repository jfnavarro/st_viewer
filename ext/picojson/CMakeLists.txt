message(STATUS "Building picojson")

# Add library and include directory
add_library(picojson INTERFACE)
target_include_directories(picojson INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
