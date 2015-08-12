#pragma once

#include <string>
namespace data_model {
  struct Everything;
}

void json_serialize(const data_model::Everything &data_model_everything, const std::string &filename);
