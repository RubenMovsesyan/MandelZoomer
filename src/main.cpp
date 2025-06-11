#include "window/MandelWindow.h"
// #include <SDL3/SDL_keycode.h>
// #include <memory>

int main() {
    // // Don't want to keep this allocated on the stack
    std::unique_ptr<MandelWindow> window = std::make_unique<MandelWindow>(1280, 720);

    window->run_mandel_zoomer();

    // window->set_key_callback(
    //     [&](
    //         SDL_Keycode key_code,
    //         SDL_Keymod key_mod,
    //         bool key_down,
    //         bool repeat
    //     ) {
    //         switch (key_code) {
    //             case SDLK_S:
    //                 printf("The S key has been pressed\n");
    //                 break;
    //             default:
    //                 printf("A key has been pressed\n");
    //                 break;
    //         }
    //     }
    // );

    // window->set_mouse_motion_callback(
    //     [&](
    //         float x,
    //         float y
    //     ) {
    //         printf("The mouse is at: %f, %f\n", x, y);
    //     }
    // );

    // window->run_mandel_zoomer();

    return 0;
}
