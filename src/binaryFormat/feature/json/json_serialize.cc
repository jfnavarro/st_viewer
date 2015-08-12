#include "json_serialize.h"


#include <binaryFormat/feature/datamodel.h>
#include <string>
#include <utility>
#include <cassert>


#include <iostream>

void json_serialize(const data_model::Everything &data_model_everything, const std::string &filename) {
  int gene_index = 0;

  std::ofstream ofile(filename);
  //  ofile.open(filename.c_str(), ios::out | ios::binary);
  ofile << "[\n";
  bool first_time = true;
  for (const auto &gene : data_model_everything.genes) {
    int y_index = 0;
    for (const auto &horizontal_line : gene.horizontal_lines) {
      for (const auto &position_hit : horizontal_line.position_hits) {
        const int x = position_hit.x;
	const auto y = y_index;
	assert(x >= 0 && x < 100000);
 	if (!first_time) {
          ofile << ",\n";
	}
        // We would like to have each entry written to one line because we could then sort
	// the lines and be able to compare json outpus where the entries were written in
	// different order
	ofile << "  {    \"y\": " << y << ",    \"x\": " << x << ",    \"hits\": " << position_hit.hits << ",    \"barcode\": \"" << data_model_everything.barcode_map.at( data_model::two_dimensional_coord_t(x, y)  ) << "\",    \"gene\": \"" << data_model_everything.gene_names[gene_index] << "\"  }";
        if (first_time) {
          first_time = false;
	}
      }
      ++y_index;
    }
    //    auto xcoord_hits = gene_builder.initXcoordHits(geneinfo.size();
    ++gene_index;
  }
  ofile << "\n]\n";
  ofile.close();
}
