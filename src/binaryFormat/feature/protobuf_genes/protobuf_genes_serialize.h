#ifndef PROTOBUF_GENES_SERIALIZE_H
#define PROTOBUF_GENES_SERIALIZE_H

#include <string>

namespace data_model {
  struct Everything;
}

void protobuf_genes_serialize(const data_model::Everything &data_model, const std::string &filename);

#endif
