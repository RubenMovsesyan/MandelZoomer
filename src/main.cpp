#define WEBGPU_CPP_IMPLEMENTATION
#include "window/MandelWindow.h"
// #include <SDL3/SDL_keycode.h>
// #include <memory>

int main() {
    // // Don't want to keep this allocated on the stack
    std::unique_ptr<MandelWindow> window = std::make_unique<MandelWindow>(1280, 720);

    window->set_key_callback(
        [&](
            int key,
            int scancode,
            int action,
            int mode
        ) {
        }
    );

    window->set_mouse_motion_callback(
        [&](
            double x,
            double y
        ) {
        }
    );

#ifdef WGPU_BACKEND_WGPU
    printf("Using wgpu backend\n");
#endif

    window->run_mandel_zoomer();

    return 0;
}
