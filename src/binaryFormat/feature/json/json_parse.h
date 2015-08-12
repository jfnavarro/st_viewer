#pragma once

#include <time.h>
#include <string>

namespace data_model {
struct Everything;
}
void json_parse(char *buf, int size, data_model::Everything *data_model);

