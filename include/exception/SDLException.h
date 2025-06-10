#ifndef SDLEXCEPTION_H
#define SDLEXCEPTION_H

#include <SDL3/SDL_error.h>
#include <format>
#include <stdexcept>

// Class to throw errors when something doesn't work with SDL
class SDLException final : public std::runtime_error {
  public:
    explicit SDLException(const std::string &message) : std::runtime_error(std::format("{}\n{}", message, SDL_GetError())) {}
};

#endif
