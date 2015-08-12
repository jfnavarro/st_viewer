#ifndef PARSERS_AND_SERIALIZERS_H
#define PARSERS_AND_SERIALIZERS_H

#include <functional>
#include <unordered_map>
#include <memory>

#include <binaryFormat/feature/datamodel.h>

#include <binaryFormat/feature/protobuf_genes/protobuf_genes_serialize.h>
#include <binaryFormat/feature/protobuf_genes/protobuf_genes_parse.h>
#include <binaryFormat/feature/protobuf_genes/genes.pb.h>
#include <binaryFormat/feature/capnproto_genes/capnproto_genes_serialize.h>
#include <binaryFormat/feature/capnproto_genes/capnproto_genes_parse.h>

#include <binaryFormat/feature/json/json_serialize.h>
#include <binaryFormat/feature/json/json_parse.h>

// Rapidjson is about twice as slow as yajl, so we just use yajl
// #include "rapidjson_parse.h"

typedef std::function< void (char *, int, data_model::Everything *) > parser_func;
typedef std::function< void (const data_model::Everything &, const std::string &) > serializer_func;
typedef std::function< std::unique_ptr< std::vector< data_model::Gene > >(const std::string &filename, const std::vector< data_model::gene_id_t > &) > retrieve_some_genes_func;

struct ParserSerializer {
  parser_func parser;
  serializer_func serializer;
};

typedef std::unordered_map< std::string, ParserSerializer > parser_serializer_map_type;

const parser_serializer_map_type parser_serializer_map = {
  {"capnproto_genes", ParserSerializer{ &capnproto_genes_parse, &capnproto_genes_serialize}},
  {"json", ParserSerializer{ &json_parse, &json_serialize}},
  {"protobuf_genes", ParserSerializer{ &protobuf_genes_parse, &protobuf_genes_serialize}
}
    
};

#endif
