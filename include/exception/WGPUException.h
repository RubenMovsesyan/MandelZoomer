#ifndef WGPUEXCEPTION_H
#define WGPUEXCEPTION_H

#include <stdexcept>

class WGPUException final : public std::runtime_error {
  public:
    explicit WGPUException(const std::string& message) : std::runtime_error(message) {}
};

#endif
