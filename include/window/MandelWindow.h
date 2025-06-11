#ifndef MANDEL_WINDOW_H
#define MANDEL_WINDOW_H

// #include "render/MandelRender.h"
#include "render/MandelRender.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <webgpu/webgpu.h>

// Type aliases
// 										code 		 modifiers  keydown repeat
using KeyCallback = std::function<void()>;
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
    WGPUInstance instance;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;

    WGPUSurface surface;

    MandelRender* renderer;

    GLFWwindow* window;

    float prev_mouse_x, prev_mouse_y;

    // Callback functions
    KeyCallback key_callback;
    MouseMotionCallback mouse_motion_callback;
    MouseMotionDeltaCallback mouse_motion_delta_callback;

    WGPUAdapter request_adapter_sync(WGPURequestAdapterOptions const* options);
    WGPUDevice request_device_sync(WGPUDeviceDescriptor const* descriptor);
};

#endif
