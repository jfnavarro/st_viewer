#ifndef MMAPPED_FILE_H
#define MMAPPED_FILE_H

#include <string>

class MmappedFile {
 public:
 
  MmappedFile(const std::string &filepath);
  ~MmappedFile();
  char *buffer();
  int size_of_buffer();

  
private:
  int fd_;
  int size_;
  char *buf_;
};

#endif
