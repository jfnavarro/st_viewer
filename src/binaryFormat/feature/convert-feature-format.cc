#include "parsers_and_serializers.h"
#include "mmapped_file.h"
std::string usage_string(const parser_serializer_map_type &map) {
  std::string result("Usage:\n"
                         "  convert-feature-format inputformat inputfile outputformat outputfile\n\n"
                         "  supported formats are:\n");
  for (const auto &pair : map) {
    result.append("    ");
    result.append(pair.first);
    result.append("\n");
  }
  return result;
}

int main(int argc, char ** argv) {
  data_model::Everything data_model_everything;
  errno = 0;
  if (argc == 2 && (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0)) {
    std::cout << usage_string(parser_serializer_map);
    return EXIT_SUCCESS;
  }
  if (argc != 5) {
    std::cerr << "incorrect usage:\n" << usage_string(parser_serializer_map);
    return EXIT_FAILURE;
  }
  char *input_file_format = argv[1];
  char *input_file_path = argv[2];
  char *output_file_format = argv[3];
  char *output_file_path = argv[4];
    
  auto parser_search = parser_serializer_map.find(input_file_format);
  if(parser_search != parser_serializer_map.end()) {
          MmappedFile mfile(input_file_path);

      parser_search->second.parser(mfile.buffer(), mfile.size_of_buffer(), &data_model_everything);
  }
  else {
    std::cerr << "inputformat \"" << input_file_format << "\" not supported\n";
    return EXIT_FAILURE;
  }
  auto serializer_search = parser_serializer_map.find(output_file_format);
  if(serializer_search != parser_serializer_map.end()) {
    if (serializer_search->second.serializer == nullptr) {
      std::cerr << "serialize to outputformat \"" << output_file_format << "\" is not supported\n";
      return EXIT_FAILURE;
    }
    serializer_search->second.serializer(data_model_everything, output_file_path);
  }
  else {
    std::cerr << "outputformat \"" << output_file_format << "\" not supported\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
