#ifndef MANDEL_WINDOW_H
#define MANDEL_WINDOW_H

// #include "render/MandelRender.h"
#include "render/MandelRender.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <webgpu/webgpu.h>

// Type aliases
using KeyCallback = std::function<void(int key, int scancode, int action, int mode)>;
// 												x pos	  y pos
using MouseMotionCallback = std::function<void(double x, double y)>;
// 													delta x		delta y
using MouseMotionDeltaCallback = std::function<void(double dx, double dy)>;

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

    double prev_mouse_x, prev_mouse_y;

    // Callback functions
    KeyCallback key_callback;
    MouseMotionCallback mouse_motion_callback;
    MouseMotionDeltaCallback mouse_motion_delta_callback;

    WGPUAdapter request_adapter_sync(WGPURequestAdapterOptions const* options);
    WGPUDevice request_device_sync(WGPUDeviceDescriptor const* descriptor);

    // Static callbacks
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void glfw_mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);

    void set_glfw_callbacks();
};

#endif
