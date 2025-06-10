#ifndef FILEIOEXCEPTION_H
#define FILEIOEXCEPTION_H

#include <stdexcept>

class FileIOException final : public std::runtime_error {
  public:
    explicit FileIOException(const std::string& message) : std::runtime_error(message) {}
};

#endif
