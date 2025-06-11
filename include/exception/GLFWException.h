#ifndef GLFWEXCEPTION_H
#define GLFWEXCEPTION_H

#include <stdexcept>

class GLFWException final : public std::runtime_error {
  public:
    explicit GLFWException(const std::string& message) : std::runtime_error(message) {}
};

#endif
