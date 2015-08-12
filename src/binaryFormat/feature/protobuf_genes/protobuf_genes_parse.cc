#include <binaryFormat/feature/protobuf_genes/protobuf_genes_parse.h>

#include <cassert>

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

void protobuf_genes_parse(char *buf, int size, data_model::Everything *data_model_everything) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  //  google::protobuf::io::CodedInputStream::SetTotalBytesLimit(1000000000, 1000000000);
  tutorial::Everything everything;
  {
    if (!everything.ParseFromArray(buf, size)) {
      throw std::runtime_error("failed to parse protobuf file");
    }
  }
  {
    {
      const auto len = everything.genenames_size();
      for (int i = 0; i < len; i++) {
	const auto &gene_name = everything.genenames(i);
	data_model_everything->gene_id_map[gene_name] = i;
	data_model_everything->gene_names.push_back(gene_name);
      }
    }
    {
      const auto len = everything.positions_size();
      for (int i = 0; i < len; i++) {
	const auto &pos = everything.positions(i);
	data_model_everything->barcode_map[data_model::two_dimensional_coord_t(pos.xcoord(), pos.ycoord())]
	  = pos.barcode();
      }
    }
    {
      const auto len = everything.genes_size();
      data_model_everything->genes.reserve(len);
      for (int i = 0; i < len; i++) {
	const auto &gene = everything.genes(i);
	data_model::Gene model_gene;
	const auto ylines_size = gene.ylines_size();
	model_gene.horizontal_lines.reserve(ylines_size);
	for (int j = 0; j < ylines_size; j++) {
	  const auto &yline = gene.ylines(j);
	  assert(yline.xcoord_size() == yline.hits_size());
	  const auto length = yline.xcoord_size();
	  data_model::HorizontalLine horizontal_line;
	  horizontal_line.position_hits.reserve(length);
	  for (int i = 0; i < length; ++i) {
	    const auto &x = yline.xcoord(i);
	    const auto &hits = yline.hits(i);
	    if (x > std::numeric_limits< data_model::coord_t >::max()) {
	      throw std::runtime_error("xcoord too big");
	    }
	    if (hits > std::numeric_limits< data_model::hits_t >::max()) {
	      throw std::runtime_error("hits too big");
	    }
	    horizontal_line.position_hits.push_back(data_model::PositionHit{ static_cast<data_model::coord_t>(x),  static_cast<data_model::hits_t>(hits)});
	  }
	  model_gene.horizontal_lines.push_back(std::move(horizontal_line));
	}
	data_model_everything->genes.push_back(std::move(model_gene));
      }
    }
  }
}
