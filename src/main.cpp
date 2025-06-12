#define WEBGPU_CPP_IMPLEMENTATION
#include "window/MandelWindow.h"
// #include <SDL3/SDL_keycode.h>
// #include <memory>

int main() {
    // // Don't want to keep this allocated on the stack
    std::unique_ptr<MandelWindow> window = std::make_unique<MandelWindow>(1280, 720);

    window->run_mandel_zoomer();

    return 0;
}
