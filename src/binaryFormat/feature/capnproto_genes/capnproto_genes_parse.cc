#include "capnproto_genes_parse.h"
#include <cassert>

#include <map>
#include <limits>
#include  <cstdint>
#include <cassert>
#include <vector>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

#include <fcntl.h>
#include <binaryFormat/feature/datamodel.h>

#include <binaryFormat/feature/capnproto_genes/genes.capnp.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <capnp/serialize.h>

#include <iostream>

data_model::Gene translate(const ::capnp_genes::Gene::Reader &gene) { 
  data_model::Gene model_gene;
  model_gene.horizontal_lines.reserve( gene.getYlinesManyHits().size());
  for (auto yline : gene.getYlinesManyHits()) {
      assert(yline.getXcoord().size() == yline.getHits().size());
      const auto length = yline.getXcoord().size();
      const auto &xcoord_vector(yline.getXcoord());
      const auto &hits_vector(yline.getHits());
      data_model::HorizontalLine horizontal_line;
      horizontal_line.position_hits.reserve(length);
      for (unsigned int i = 0; i < length; ++i) {
	horizontal_line.position_hits.push_back(data_model::PositionHit{ xcoord_vector[i], hits_vector[i]});
      }
      model_gene.horizontal_lines.push_back(std::move(horizontal_line));
  }
  return model_gene;
}

void capnproto_genes_parse(char *buf, int size, data_model::Everything *data_model_everything) {
  assert(data_model_everything);
  capnp::ReaderOptions options;
  // Default value:
  // $ grep -r traversalLimitInWords capnproto-c++-0.5.1.2/inst/include/capnp/message.h
  // uint64_t traversalLimitInWords = 8 * 1024 * 1024;
  options.traversalLimitInWords = 20 * 8 * 1024 * 1024;
  capnp::FlatArrayMessageReader message(kj::arrayPtr(reinterpret_cast<const capnp::word*>(buf),
						     size / sizeof(capnp::word)), options
					);
  capnp_genes::Everything::Reader everything = message.getRoot<capnp_genes::Everything>();
  {
    int index1 = 0;
    for (auto gene_name : everything.getGeneNames()) {
      data_model_everything->gene_id_map[gene_name] = index1;
      data_model_everything->gene_names.push_back(gene_name);
      ++index1;
    }
  }
  for (auto position : everything.getPositions()) {
    data_model_everything->barcode_map[ data_model::two_dimensional_coord_t( position.getXcoord(), position.getYcoord())]
      = position.getBarcode();
  }
  data_model_everything->genes.reserve(everything.getGenes().size());
   for (auto gene : everything.getGenes()) {
    data_model_everything->genes.push_back(translate(gene));
  }
}

std::vector< data_model::Gene > capnproto_genes_retrieve_some_genes(char *buf, int size, const std::vector< data_model::gene_id_t > &gene_id_vector) {
  capnp::ReaderOptions options;
  // Default value:
  // $ grep -r traversalLimitInWords capnproto-c++-0.5.1.2/inst/include/capnp/message.h
  // uint64_t traversalLimitInWords = 8 * 1024 * 1024;
  options.traversalLimitInWords = 20 * 8 * 1024 * 1024;
  //  ::capnp::PackedFdMessageReader message(fd, options);
  capnp::FlatArrayMessageReader message(
					kj::arrayPtr(reinterpret_cast<const capnp::word*>(buf),
						     size / sizeof(capnp::word)), options);
  capnp_genes::Everything::Reader everything = message.getRoot<capnp_genes::Everything>();
  std::vector< data_model::Gene > retrieved_genes_vector;
  for (auto gene_id : gene_id_vector) {
    data_model::Gene mme =  translate(everything.getGenes()[gene_id]);
    retrieved_genes_vector.push_back(std::move(mme));    
  }
  return retrieved_genes_vector;
}
