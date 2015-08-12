find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS genes.proto)

add_library(protobuf_genes
  STATIC
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    protobuf_genes_serialize.cc
    protobuf_genes_parse.cc
)

target_include_directories(protobuf_genes PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(protobuf_genes PUBLIC ${PROTOBUF_LITE_LIBRARIES} ${PROTOBUF_LIBRARIES})

set_property(TARGET protobuf_genes PROPERTY CXX_STANDARD 11)
set_property(TARGET protobuf_genes PROPERTY CXX_STANDARD_REQUIRED ON)

