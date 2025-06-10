#ifndef MANDEL_WINDOW_H
#define MANDEL_WINDOW_H

#include "render/MandelRender.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <functional>

// Type aliases
// 										code 		 modifiers  keydown repeat
using KeyCallback = std::function<void(SDL_Keycode, SDL_Keymod, bool, bool)>;
// 												x pos	  y pos
using MouseMotionCallback = std::function<void(float x, float y)>;
// 													delta x		delta y
using MouseMotionDeltaCallback = std::function<void(float dx, float dy)>;

class MandelWindow {
  public:
    MandelWindow(const int width, const int height);
    ~MandelWindow();

    // Main window loop
    // call after all setup has been complete
    void run_mandel_zoomer();

    // Callback functions for window events =======================================

    // Sets the window callback for a key event
    void set_key_callback(KeyCallback key_callback);
    void set_mouse_motion_callback(MouseMotionCallback mouse_motion_callback);
    void set_mouse_motion_delta_callback(MouseMotionDeltaCallback mouse_motion_callback);

  private:
    SDL_Window* window;
    MandelRender renderer;
    bool done;

    float prev_mouse_x, prev_mouse_y;

    // Callback functions
    KeyCallback key_callback;
    MouseMotionCallback mouse_motion_callback;
    MouseMotionDeltaCallback mouse_motion_delta_callback;
};

#endif
