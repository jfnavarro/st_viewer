#include "mmapped_file.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <cstdint>
#include <iostream>

#include <string>
MmappedFile::MmappedFile(const std::string &filepath) {
  int fd = open(filepath.c_str(), O_RDONLY);
  if(fd == -1)  {
    std::cerr << "  int fd = open(filepath.c_str(), O_RDONLY); == -1" << std::endl;
    throw std::runtime_error(strerror(errno));
  }
  struct stat file_stat;
  int status = fstat(fd, &file_stat);
  if (status != 0) {
    throw std::runtime_error(strerror(errno));
  }
   auto result =mmap (0, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (result == MAP_FAILED) {
         throw std::runtime_error(strerror(errno));
    }
  buf_ = static_cast< char *>(result);
  size_ = file_stat.st_size;
  fd_ = fd;
}

MmappedFile::~MmappedFile() {
   {
  int unmap_result = munmap(buf_, size_);
  if (unmap_result != 0) {
    throw std::runtime_error(strerror(errno));
  }
  }
   if (close(fd_) == -1) {
         throw std::runtime_error(strerror(errno));
  }
}

char *MmappedFile::buffer() {
  return buf_;
}
int MmappedFile::size_of_buffer() {
  return size_;
}
