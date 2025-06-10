#include "window/MandelWindow.h"
#include "exception/SDLException.h"
#include "render/MandelRender.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_video.h>

MandelWindow::MandelWindow(const int width, const int height) {
    // Init SDL
    // Init the video output
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw SDLException("Couldn't Initialize SDL");
    }

    // Create the window and start the application loop
    this->window = SDL_CreateWindow("MandelZoomer", width, height, 0);

    // Initialize other variables
    this->prev_mouse_x = 0.0;
    this->prev_mouse_y = 0.0;

    // If the window does not exist then dont start the application loop
    if (this->window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());

        throw SDLException("Couldn't Initialize Window");

        this->done = true;
    } else {
        this->done = false;
    }

    this->renderer = MandelRender(this->window);
}

MandelWindow::~MandelWindow() {
    // Destruct the renderer so it drops the window before it goes out of scope
    this->renderer.~MandelRender();

    // Destroy the window and cleanup
    if (this->window != NULL) {
        SDL_DestroyWindow(this->window);
        printf("SDL Window Destroyed\n");
    }

    // Gracefully quit SDL
    SDL_Quit();
    printf("Goodbye :)\n");
}

void MandelWindow::run_mandel_zoomer() {
    while (!done) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT: {
                    done = true;
                    break;
                }
                case SDL_EVENT_KEY_DOWN: {
                    SDL_KeyboardEvent keyboard_event = event.key;
                    if (this->key_callback) {
                        this->key_callback(keyboard_event.key, keyboard_event.mod, keyboard_event.down, keyboard_event.repeat);
                    }

                    break;
                }
                case SDL_EVENT_MOUSE_MOTION: {
                    SDL_MouseMotionEvent mouse_motion_event = event.motion;
                    if (this->mouse_motion_callback) {
                        this->mouse_motion_callback(mouse_motion_event.x, mouse_motion_event.y);
                    }

                    if (this->mouse_motion_delta_callback) {
                        float delta_x = prev_mouse_x - mouse_motion_event.x;
                        float delta_y = prev_mouse_y - mouse_motion_event.y;
                        this->mouse_motion_delta_callback(delta_x, delta_y);
                    }

                    prev_mouse_x = mouse_motion_event.x;
                    prev_mouse_y = mouse_motion_event.y;

                    break;
                }
                default: {
                    break;
                }
            }
        }

        // Call the render pass of the renderer
        this->renderer.render();
    }
}

// Function implementaitons to set the callbacks for each window event
void MandelWindow::set_key_callback(KeyCallback key_callback) {
    if (this->key_callback) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Existing Key Callback is being replaced\n");
    }

    this->key_callback = key_callback;
}

void MandelWindow::set_mouse_motion_callback(
    MouseMotionCallback mouse_motion_callback
) {
    if (this->mouse_motion_callback) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Existing Mouse Motion Callback is being replaced\n");
    }

    this->mouse_motion_callback = mouse_motion_callback;
}

void MandelWindow::set_mouse_motion_delta_callback(
    MouseMotionDeltaCallback mouse_motion_delta_callback
) {
    if (this->mouse_motion_delta_callback) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Existing Mouse Motion Delta Callback is being replaced\n");
    }

    this->mouse_motion_delta_callback = mouse_motion_delta_callback;
}
