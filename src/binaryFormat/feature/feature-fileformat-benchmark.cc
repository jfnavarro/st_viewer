#include <time.h>
#include <sstream>
#include <iomanip> 
#include "parsers_and_serializers.h"
#include "mmapped_file.h"

double cpu_time_used(const clock_t &start) {
  clock_t end = clock();
  return static_cast<double>( (end - start)) / CLOCKS_PER_SEC;
}

std::string usage_string() {
  std::string result("Usage:\n"
                         "feature-fileformat-benchmark json-file-path\n");
  return result;
}
 
double benchmark_parse(parser_func func, const std::string &filepath, data_model::Everything *model) {
      clock_t parse_start;
      parse_start = clock();
      MmappedFile mfile(filepath);
      func(mfile.buffer(), mfile.size_of_buffer(), model);
      return cpu_time_used(parse_start);
}

int get_compressed_size(const std::string &filepath) {
  FILE *in;
  char buff[512];
  std::string command = std::string("cat ") + filepath + std::string(" | gzip - | wc -c");;
  if(!(in = popen(command.c_str(), "r"))){
    throw std::runtime_error("could not open pipe");
  }
  std::ostringstream sstream;
  while(fgets(buff, sizeof(buff), in) != NULL){
     sstream << buff;
  }
  pclose(in);
  return std::stoi(   sstream.str() );
}

int main(int argc, char ** argv) {
  errno = 0;
  if (argc == 2 && (strcmp(argv[1],"--help") == 0 || strcmp(argv[1],"-h") == 0)) {
    std::cout << usage_string();
    return EXIT_SUCCESS;
  }
  if (argc != 2) {
    std::cerr << "incorrect usage:\n" << usage_string();
    return EXIT_FAILURE;
  }

  char *json_filepath = argv[1];
  data_model::Everything data_model_everything;

  try {
  
  {
  MmappedFile mfile(json_filepath);
  json_parse(mfile.buffer(), mfile.size_of_buffer(), &data_model_everything);
  }
  char templ[] = "/tmp/benchmark.XXXXXX";
  char *tmpdir = mkdtemp(templ);
  if (!tmpdir) {
    std::cerr << "Could not create temp dir:\n";
    return EXIT_FAILURE;
  }

  for (const auto &entry : parser_serializer_map) {
    if (entry.second.serializer != nullptr) {
      std::string serialize_file = std::string(tmpdir) + std::string("/") + entry.first;

      clock_t serialize_start;
      serialize_start = clock();

      entry.second.serializer(data_model_everything, serialize_file.c_str());
      const auto serialize_time = cpu_time_used(serialize_start);
      struct stat statbuf;
      if (stat(serialize_file.c_str(), &statbuf) == -1) {
        std::cerr << "Could not stat file:\n";
        return EXIT_FAILURE;
      }
      data_model::Everything data_model_everything2;
      double parse_time = benchmark_parse(entry.second.parser, serialize_file.c_str(), &data_model_everything2);
      int compressed_size = get_compressed_size(serialize_file);
      std::cout.width(12);
      std::cout.setf( std::ios::fixed, std:: ios::floatfield );
      std::cout << std::setw(15) << entry.first
	        << "   serialize time (s) = " << std::setw(6) << serialize_time <<  "   parse time = " <<  std::setw(6) << parse_time <<  "     compressed size = " << compressed_size << "   parse time  = " <<   std::setw(6) << parse_time << std::endl;
      std::string result_file = std::string(tmpdir) + std::string("/") + entry.first;
    }
  }
  }

    catch (const std::runtime_error& e) {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
    }
    catch (...) {
      std::cerr << "error: unknown exception thrown" << std::endl;
            return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
