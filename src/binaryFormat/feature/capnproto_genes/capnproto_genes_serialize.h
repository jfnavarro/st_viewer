#ifndef CAPNPROTO_GENES_SERIALIZE_H
#define CAPNPROTO_GENES_SERIALIZE_H

#include <string>

namespace data_model {
  struct Everything;
}

void capnproto_genes_serialize(const data_model::Everything &data_model_everything, const std::string &filename);

#endif
