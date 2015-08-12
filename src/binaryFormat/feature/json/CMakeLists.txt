set(YAJL_LIBRARIES /home/esjolund/tmp/yajl-2.1.0/inst/lib/libyajl.so)
set(YAJL_INCLUDE_DIRS /home/esjolund/tmp/yajl-2.1.0/inst/include/)

add_library(json
  STATIC
    json_serialize.cc
    json_parse.cc
  )
target_include_directories(json PUBLIC ${YAJL_INCLUDE_DIRS} ${YAJL_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
target_link_libraries(json PUBLIC ${YAJL_LIBRARIES})
