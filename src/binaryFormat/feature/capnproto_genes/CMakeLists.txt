find_package(CapnProto REQUIRED)

set(CAPNPC_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS genes.capnp)

add_library(capnproto_genes
  SHARED
  ${CAPNP_SRCS}
  ${CAPNP_HDRS}
  capnproto_genes_serialize.cc
  capnproto_genes_parse.cc
)

target_include_directories(capnproto_genes PUBLIC ${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src ${CAPNP_INCLUDE_DIRS})
target_link_libraries(capnproto_genes PUBLIC ${CAPNP_LIBRARIES})
target_compile_definitions(capnproto_genes PUBLIC CAPNP_HEADER_WARNINGS=0 KJ_HEADER_WARNINGS=0 ${CAPNP_DEFINITIONS})
set_property(TARGET capnproto_genes PROPERTY CXX_STANDARD 11)
set_property(TARGET capnproto_genes PROPERTY CXX_STANDARD_REQUIRED ON)
