message(STATUS "Building picojson")

set(SRC_libpicojson
    ${PROJECT_SOURCE_DIR}/ext/picojson/picojson.h
)

# Add library and include directory
add_library(picojson OBJECT ${SRC_picojson})
include_directories(${PROJECT_SOURCE_DIR}/ext/picojson)

# Organize lib into solution folder
project_group(picojson "ext")
