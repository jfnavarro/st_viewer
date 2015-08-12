set(formats protobuf_genes capnproto_genes json)

add_library(feature_parse_and_serialize STATIC
  mmapped_file.cc
  parsers_and_serializers.h
  fillFeatureListAndGeneListFromDataModel.cc
)

foreach(i ${formats})
  add_subdirectory(${i})
#  file(APPEND ${formats}/ "parser_serializer_map[\"${i}\"]= ParserSerializer{ &capnp_genes_parse, &capnp_genes_serialize};")
  target_link_libraries(feature_parse_and_serialize PUBLIC ${i})
endforeach()

set_property(TARGET feature_parse_and_serialize PROPERTY CXX_STANDARD 11)
set_property(TARGET feature_parse_and_serialize PROPERTY CXX_STANDARD_REQUIRED ON)
#    $<TARGET_OBJECTS:feature_parse_and_serialize>
macro(feature_executable executable_name)
add_executable(${executable_name}
  ${executable_name}.cc
)

target_link_libraries(${executable_name} PUBLIC feature_parse_and_serialize)
set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${executable_name} PROPERTY CXX_STANDARD_REQUIRED ON)
#  target_link_libraries(${executable_name} "${YAJL_LIBRARIES}")
#  target_link_libraries(${executable_name} ${PROTOBUF_LITE_LIBRARIES} ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES})
endmacro()

feature_executable(convert-feature-format)
feature_executable(feature-fileformat-benchmark)


string (REPLACE ";" " " format_list_separated_by_spaces "${formats}")
set(tmpfile ${CMAKE_CURRENT_BINARY_DIR}/verify-formats.sh.file-generate)
configure_file(verify-formats.sh.cmake ${tmpfile})

file(GENERATE OUTPUT verify-formats.sh INPUT ${tmpfile})
