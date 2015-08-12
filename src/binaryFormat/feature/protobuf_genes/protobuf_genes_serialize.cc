#include "protobuf_genes_serialize.h"

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
#include <binaryFormat/feature/protobuf_genes/genes.pb.h>

void protobuf_genes_serialize(const data_model::Everything &data_model_everything, const std::string &filename) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  tutorial::Everything everything;
  for (auto &iter : data_model_everything.gene_names) {
    std::string *str = everything.add_genenames();
    *str = iter;
  };
  for (auto &iter : data_model_everything.barcode_map) {
    tutorial::Position *pos = everything.add_positions();
    pos->set_barcode(iter.second);
    pos->set_xcoord(iter.first.first);
    pos->set_ycoord(iter.first.second);
  };
  int gene_index = 0;
  for (const auto &gene : data_model_everything.genes) {
    tutorial::Gene *gen = everything.add_genes();
    int y_index = 0;
    for (const auto &horizontal_line : gene.horizontal_lines) {
      tutorial::Yline *yline = gen->add_ylines();
      int position_hit_index = 0;
      for (const auto &position_hit : horizontal_line.position_hits) {
        yline->add_xcoord(position_hit.x);
        yline->add_hits(position_hit.hits);
	++position_hit_index;
      }
      ++y_index;
    }
    ++gene_index;
  }
  {
    std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!everything.SerializeToOstream(&output)) {
      std::cerr << "Failed to write address book." << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  //  google::protobuf::ShutdownProtobufLibrary();

}
