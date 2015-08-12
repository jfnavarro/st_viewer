#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <unordered_map>
#include <map>
#include <limits>
#include <cstdint>
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

#include <fstream>
#include <err.h>

namespace data_model {

typedef uint16_t gene_id_t;
typedef uint16_t coord_t;
typedef uint32_t hits_t;

typedef std::pair< coord_t, coord_t > two_dimensional_coord_t;

struct PositionHit {
  coord_t x;
  hits_t hits;
};

struct HorizontalLine {
  std::vector< PositionHit > position_hits;
};

struct Gene {
  std::vector< HorizontalLine > horizontal_lines;
};

struct Everything {
  ::std::unordered_map< std::string, gene_id_t > gene_id_map;
  std::vector< std::string > gene_names;
  std::map< two_dimensional_coord_t, std::string > barcode_map;
  std::vector< Gene > genes;
};

struct GeneRead {
  coord_t x;
  coord_t y;
  hits_t hits;
  std::string gene_name;
  std::string barcode;
};

}
