#include "capnproto_genes_serialize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <map>
#include <limits>
#include  <cstdint>
#include <cassert>
#include <vector>
#include <string.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <binaryFormat/feature/datamodel.h>
#include <binaryFormat/feature/capnproto_genes/genes.capnp.h>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>

#include <algorithm>

void capnproto_genes_serialize(const data_model::Everything &data_model_everything, const std::string &filename) {
  ::capnp::MallocMessageBuilder message;
  capnp_genes::Everything::Builder everything = message.initRoot< ::capnp_genes::Everything>();
  ::capnp::List<::capnp::Text>::Builder gene_names = everything.initGeneNames(data_model_everything.gene_names.size());

  {
  int index2 = 0 ;
  for (auto &iter : data_model_everything.gene_names) {
    gene_names.set(index2, iter);
    ++index2;
  };
  }
  auto positions_builder = everything.initPositions(data_model_everything.barcode_map.size());
  int positions_index = 0 ;
  for (auto &iter : data_model_everything.barcode_map) {
    auto position =  positions_builder[positions_index];
    position.setBarcode(iter.second);
    position.setXcoord(iter.first.first);
    position.setYcoord(iter.first.second);
    ++positions_index;
  };
  auto genes_builder = everything.initGenes(data_model_everything.genes.size());
  int gene_index = 0;
  for (const auto &gene : data_model_everything.genes) {
    auto y_lines = genes_builder[ gene_index ].initYlinesManyHits(gene.horizontal_lines.size()) ;
    int y_index = 0;
    for (const auto &horizontal_line : gene.horizontal_lines) {
      const auto len(horizontal_line.position_hits.size());
      auto xcoords = y_lines[y_index].initXcoord(len);
      auto hits =  y_lines[y_index].initHits(len);
      int position_hit_index = 0;
      for (const auto &position_hit : horizontal_line.position_hits) {
        hits.set(position_hit_index, position_hit.hits);
        xcoords.set(position_hit_index, position_hit.x);
	++position_hit_index;
      }
      ++y_index;
    }
    ++gene_index;
  }
  int fd_write = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644 );
  if (fd_write == -1) {
    std::cerr << "file=" << filename << "open (filename, O_WRONLY | O_CREAT | O_TRUNC ) == -1\n";
    exit(EXIT_FAILURE);
  }
  writeMessageToFd(fd_write, message);
  close(fd_write); 
}
