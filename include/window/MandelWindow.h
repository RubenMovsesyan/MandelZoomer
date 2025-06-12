#ifndef MANDEL_WINDOW_H
#define MANDEL_WINDOW_H

// #include "render/MandelRender.h"
#include "render/MandelRender.h"
#include "webgpu/webgpu.hpp"
#include <GLFW/glfw3.h>
#include <functional>

// Type aliases
using KeyCallback = std::function<void(int key, int scancode, int action, int mode)>;
// 												x pos	  y pos
using MouseMotionCallback = std::function<void(double x, double y)>;
// 													delta x		delta y
using MouseMotionDeltaCallback = std::function<void(double dx, double dy)>;

using MouseScrollCallback = std::function<void(double dx, double dy)>;

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
    void set_mouse_scroll_callback(MouseScrollCallback mouse_scroll_callback);

  private:
    wgpu::Instance instance;
    wgpu::Adapter adapter;
    wgpu::Device device;
    wgpu::Queue queue;

    wgpu::Surface surface;

    MandelRender* renderer;

    GLFWwindow* window;

    double prev_mouse_x, prev_mouse_y;

    // Callback functions
    KeyCallback key_callback;
    MouseMotionCallback mouse_motion_callback;
    MouseMotionDeltaCallback mouse_motion_delta_callback;
    MouseScrollCallback mouse_scroll_callback;

    wgpu::Adapter request_adapter_sync(wgpu::RequestAdapterOptions const* options);
    wgpu::Device request_device_sync(wgpu::DeviceDescriptor const* descriptor);

    // Static callbacks
    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
    static void glfw_mouse_motion_callback(GLFWwindow* window, double xpos, double ypos);
    static void glfw_scroll_motion_callback(GLFWwindow* window, double x_offset, double y_offset);

    void set_glfw_callbacks();
};

#endif
