#ifndef CAPNPROTO_GENES_PARSE_H
#define CAPNPROTO_GENES_PARSE_H
#include <time.h>
#include <string>
#include <memory>
#include <vector>
#include <binaryFormat/feature/datamodel.h>

void capnproto_genes_parse(char *buf, int size, data_model::Everything *data_model_everything);

std::vector< data_model::Gene >  capnproto_genes_retrieve_some_genes(char *buf, int size, const std::vector< data_model::gene_id_t > &gene_id_vector);

#endif
