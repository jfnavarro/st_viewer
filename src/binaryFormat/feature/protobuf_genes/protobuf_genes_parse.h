#ifndef PROTOBUF_GENES_PARSE_H
#define PROTOBUF_GENES_PARSE_H

namespace data_model {
  struct Everything;
}

void protobuf_genes_parse(char *buf, int size, data_model::Everything *data_model);

#endif
