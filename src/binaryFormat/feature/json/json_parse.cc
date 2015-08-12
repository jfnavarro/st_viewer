/*
 * Copyright (c) 2007-2014, Lloyd Hilaiel <me@lloyd.io>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include "json_parse.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

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

#include <fstream>
#include <err.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <zlib.h>

#include <binaryFormat/feature/datamodel.h>

//protobuf

int my_strncmp( const unsigned char *s1, const char *s2, size_t n) {
  return strncmp(reinterpret_cast<const char *>(s1), s2, n);

}

struct ParserContext {
  enum class State {
    X_COORD,
      Y_COORD,
      HITS,
      BARCODE,
      GENE,
      NONE
      };
  data_model::Everything *data_model;
  State state = State::NONE;
  data_model::coord_t tmp_x;
  data_model::coord_t tmp_y;
  std::string tmp_barcode;
  data_model::hits_t tmp_hits;
  data_model::gene_id_t tmp_gene_id;
};

ParserContext *getContext(void *ctx) {
  assert(ctx);
  return static_cast< ParserContext *>(ctx);
}

ParserContext::State getState(const unsigned char * stringVal,
			      size_t stringLen) {
  if (stringLen == 1) {
    if (*stringVal == 'x') {
      return ParserContext::State::X_COORD;
    }
    if (*stringVal == 'y') {
      return ParserContext::State::Y_COORD;
    }
  }
  if (stringLen == 4) {
    if (my_strncmp(stringVal, "hits", 4) == 0) {
      return ParserContext::State::HITS;
    }
    if (my_strncmp(stringVal, "gene", 4) == 0) {
      return ParserContext::State::GENE;
    }
  }
  if (stringLen == 7 && my_strncmp(stringVal, "barcode", 7) == 0) {
    return ParserContext::State::BARCODE;
  }
  throw std::runtime_error("Not recognized json map key");

  assert(false);
}

static int reformat_null(void * /* ctx */)
{
  assert(false);
  return true;  
}

static int reformat_boolean(void * /* ctx */, int /* boolean */)
{
  assert(false);
  return true;
}

static int reformat_number(void * ctx, const char * s, size_t l)
{
  auto context(getContext(ctx));
  assert(context->state != ParserContext::State::NONE);
  unsigned long num = std::stoul(std::string(reinterpret_cast<const char *>(s), l));
  assert(num >= 0);
  if (context->state == ParserContext::State::X_COORD || context->state == ParserContext::State::Y_COORD) {
    assert(std::numeric_limits< data_model::coord_t >::max() >= num);
    if (context->state == ParserContext::State::X_COORD) {
      context->tmp_x = num;
      return true;
    }
    if (context->state == ParserContext::State::Y_COORD) {
      context->tmp_y = num;
      return true;
    }
  }
  if (context->state == ParserContext::State::HITS) {
    assert(std::numeric_limits< data_model::hits_t >::max() >= num);
    context->tmp_hits = num;
    return true;
  }
  assert(false);
  return true;
}

static int reformat_string(void * ctx, const unsigned char * stringVal,
                           size_t stringLen)
{
  auto context(getContext(ctx));
  assert(context->state != ParserContext::State::NONE);
  auto &data_model(  *(context->data_model));
  if (context->state == ParserContext::State::BARCODE) {
    context->tmp_barcode = std::string(reinterpret_cast<const char *>(stringVal), stringLen);
    return true;
  }
  if (context->state == ParserContext::State::GENE) {
    std::string gene(reinterpret_cast<const char *>(stringVal), stringLen);
    auto &gene_id_map(data_model.gene_id_map);
    //    auto &id_gene_map(data_model.id_gene_map);
    //    std::cerr << "find " << gene << "\n";
    auto it = gene_id_map.find(gene);
    if (it == gene_id_map.end()) {
      //   std::cerr << "not found " << gene << "\n";

      //    std::cerr << "data_model.gene_names.size()=" << data_model.gene_names.size() << "\n";
      const auto len = data_model.gene_names.size();
      context->tmp_gene_id = len;
      gene_id_map[gene] = len;
      data_model.gene_names.push_back(gene);
    } else {
      context->tmp_gene_id = gene_id_map[gene];
    }
    return true;
  }

  //  std::cerr <<  std::string(reinterpret_cast<const char *>(stringVal), stringLen) << "\n";
  assert(false);
  return true;
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal,
                            size_t stringLen)
{
  assert(stringLen > 0);
  //  std::cerr << "in reformat_map_key\n";
  auto context(getContext(ctx));
  context->state = getState(stringVal, stringLen);
  assert(context->state != ParserContext::State::NONE);
  return true;
}

static int reformat_start_map(void * /* ctx */)
{
  return true;
}

static int reformat_end_map(void * ctx)
{
  auto context(getContext(ctx));
  auto &barcode_map(context->data_model->barcode_map);
  data_model::two_dimensional_coord_t two_dimensional_coord(context->tmp_x, context->tmp_y);
  auto it = barcode_map.find(two_dimensional_coord);
  //  GeneHits gene_hits(context->tmp_gene_id, context->tmp_hits);
  if (it == barcode_map.end()) {
    //    std::string barcode;
    barcode_map[two_dimensional_coord] = context->tmp_barcode;
    //, std::vector< GeneHits >({ gene_hits}) };
  } else {
    assert(barcode_map[two_dimensional_coord] == context->tmp_barcode);
    //    barcode_map[two_dimensional_coord].gene_hits.push_back(gene_hits);
  }
  auto &genes(context->data_model->genes);
  if (context->data_model->genes.size() == context->tmp_gene_id) {
    genes.push_back(data_model::Gene());  
  }

  assert(context->tmp_gene_id < context->data_model->genes.size());

  assert( context->tmp_gene_id < genes.size() );
  assert( context->tmp_gene_id >= 0 );

  auto &horizontal_lines(genes[context->tmp_gene_id].horizontal_lines);
  int diff_to_current_size = context->tmp_y - horizontal_lines.size()  + 1;
  if (diff_to_current_size > 0) {

    for (int i = 0; i < diff_to_current_size; ++i) {

      horizontal_lines.push_back(data_model::HorizontalLine());
    }
  } 
  assert(context->tmp_y <  horizontal_lines.size());
  horizontal_lines[context->tmp_y].position_hits.push_back(data_model::PositionHit{context->tmp_x, context->tmp_hits});
  return true;
}

static int reformat_start_array(void * /* ctx */)
{
  return true;
}

static int reformat_end_array(void * /* ctx */)
{
  return true;
}


static yajl_callbacks callbacks = {
  reformat_null,
  reformat_boolean,
  NULL,
  NULL,
  reformat_number,
  reformat_string,
  reformat_start_map,
  reformat_map_key,
  reformat_end_map,
  reformat_start_array,
  reformat_end_array
};

/*

static yajl_callbacks callbacks = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
*/

void checkStatus(const yajl_status& status, yajl_gen g, yajl_handle handle, const unsigned char *jsonText,
		                                             size_t jsonTextLength) {
  if (status != yajl_status_ok) {
    unsigned char *str = yajl_get_error(handle, 1, jsonText, jsonTextLength);
    std::string error_str(reinterpret_cast<const char *>(str));
    yajl_free_error(handle, str);
    yajl_gen_free(g);
    yajl_free(handle);
    throw std::runtime_error(error_str);
  }
}




void json_parse(char *buf, int size, data_model::Everything *data_model) {


  //buf contains len bytes of decompressed data



  ParserContext parser_context;
  parser_context.data_model = data_model;
  yajl_handle hand;
  //  static unsigned char fileData[65536];




  /* generator config */
  yajl_gen g;
  yajl_status stat;
  //  size_t rd;
  g = yajl_gen_alloc(NULL);
  yajl_gen_config(g, yajl_gen_beautify, 1);
  yajl_gen_config(g, yajl_gen_validate_utf8, 1);
  /* ok.  open file.  let's read and parse */
  hand = yajl_alloc(&callbacks, NULL, (void *) &parser_context);
  /* and let's allow comments by default */
  yajl_config(hand, yajl_allow_comments, 1);
  /* check arguments.*/


  /*
  int rd;
  while (!gzeof(fi)) {
    //  rd = fread((void *) fileData, 1, , file_handle);
    rd = gzread(fi, reinterpret_cast< char * >(fileData), sizeof(fileData) - 1 );
    if (rd == 0) {
      if (!gzeof(fi)) {
	fprintf(stderr, "error on file read.\n");
	exit(EXIT_FAILURE);
      }
      break;
    }
    fileData[rd] = 0;
    stat = yajl_parse(hand, fileData, rd);
    if (stat != yajl_status_ok) break;
  }
  */

  const unsigned char *buff_cast = reinterpret_cast<const unsigned char *>(buf);
  stat = yajl_parse(hand, buff_cast, size);

  checkStatus(stat, g, hand, buff_cast, size);
  //  stat = yajl_parse(hand, reinterpret_cast<  const unsigned char *> (mem),static_cast< size_t > ( stat_buf.st_size));
  //        if (stat != yajl_status_ok) break;
  stat = yajl_complete_parse(hand);
  checkStatus(stat, g, hand, buff_cast, size);
  yajl_gen_free(g);
  yajl_free(hand);
  //  gzclose(fi);
}
