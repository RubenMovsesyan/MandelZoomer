#include "window/MandelWindow.h"
#include "exception/GLFWException.h"
#include "exception/WGPUException.h"
#include "render/MandelRender.h"
#include <format>
#include <glfw3webgpu.h>

MandelWindow::MandelWindow(const int width, const int height) {
    // Initialize glfw
    if (!glfwInit()) {
        throw GLFWException("Could Not Initialize GLFW");
    }

    printf("GLFW Initialized\n");

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    printf("GLFW Version: %d.%d.%d\n", major, minor, rev);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    this->window = glfwCreateWindow(1280, 720, "Mandel Zoomer", nullptr, nullptr);

    wgpu::InstanceDescriptor desc = {};
    desc.nextInChain = nullptr;

    this->instance = wgpuCreateInstance(&desc);

    if (!this->instance) {
        throw WGPUException("Instance could not be created");
    }

    // Get the windows compatible surface
    this->surface = glfwCreateWindowWGPUSurface(this->instance, this->window);
    if (!this->surface) {
        const char* description;
        int code = glfwGetError(&description);
        throw WGPUException(std::format("Surface could not be created: {} ({})", description, code));
    }

    // Options for the adapter
    wgpu::RequestAdapterOptions adapter_options = {};
    adapter_options.nextInChain = nullptr;
    adapter_options.compatibleSurface = this->surface;

    this->adapter = this->request_adapter_sync(&adapter_options);
    printf("Adapter Request Complete\n");

    // Options for the device
    wgpu::DeviceDescriptor device_descriptor = {};

    this->device = this->request_device_sync(&device_descriptor);
    printf("Device Request Complete\n");

    this->queue = wgpuDeviceGetQueue(this->device);

    if (!this->queue) {
        throw WGPUException("Could Not Get the WebGPU Queue");
    }

    this->renderer = new MandelRender(this->surface, this->adapter, this->device, this->queue);
}

wgpu::Adapter MandelWindow::request_adapter_sync(wgpu::RequestAdapterOptions const* options) {
    struct UserData {
        wgpu::Adapter adapter = nullptr;
        bool request_ended = false;
    };

    UserData user_data;

    // Create a callback for the InstanceRequestAdapter
    auto on_adapter_request_ended = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, WGPU_NULLABLE void* p_user_data, WGPU_NULLABLE void* p_user_data_2) {
        UserData& user_data = *reinterpret_cast<UserData*>(p_user_data);

        if (status == WGPURequestAdapterStatus_Success) {
            user_data.adapter = adapter;
        } else {
            throw WGPUException(std::format("Could Not Get WebGPU Adapter: {}", message.data));
        }

        user_data.request_ended = true;
    };

    wgpu::RequestAdapterCallbackInfo callback_info;
    callback_info.mode = WGPUCallbackMode_AllowSpontaneous;
    callback_info.callback = on_adapter_request_ended;
    callback_info.userdata1 = &user_data;
    callback_info.userdata2 = nullptr;

    // Request the adapter
    wgpuInstanceRequestAdapter(
        this->instance,
        options,
        callback_info
    );

    return user_data.adapter;
}

wgpu::Device MandelWindow::request_device_sync(wgpu::DeviceDescriptor const* descriptor) {
    struct UserData {
        wgpu::Device device = nullptr;
        bool request_ended = false;
    };

    UserData user_data;

    // Create a callback for the AdapterRequestDevice
    auto on_device_request_ended = [](WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, WGPU_NULLABLE void* p_user_data, WGPU_NULLABLE void* p_user_data_2) {
        UserData& user_data = *reinterpret_cast<UserData*>(p_user_data);

        if (status == WGPURequestDeviceStatus_Success) {
            user_data.device = device;
        } else {
            throw WGPUException(std::format("Could Not Get WebGPU Device: {}", message.data));
        }

        user_data.request_ended = true;
    };

    WGPURequestDeviceCallbackInfo callback_info;
    callback_info.mode = WGPUCallbackMode_AllowSpontaneous;
    callback_info.callback = on_device_request_ended;
    callback_info.userdata1 = &user_data;
    callback_info.userdata2 = nullptr;

    // Request the device
    wgpuAdapterRequestDevice(
        this->adapter,
        descriptor,
        callback_info
    );

    return user_data.device;
}

MandelWindow::~MandelWindow() {
    delete this->renderer;

    wgpuInstanceRelease(this->instance);
    wgpuQueueRelease(this->queue);
    wgpuDeviceRelease(this->device);
    wgpuAdapterRelease(this->adapter);
    wgpuSurfaceRelease(this->surface);

    glfwTerminate();
}

/*
 *
 *
 * This is the Main application loop
 *
 *
 */
void MandelWindow::run_mandel_zoomer() {
    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();

        this->renderer->render();
    }
}

// Function implementaitons to set the callbacks for each window event
void MandelWindow::set_key_callback(KeyCallback key_callback) {
    if (this->key_callback) {
        printf("Existing Key Callback is being replaced\n");
    }

    this->key_callback = key_callback;

    this->set_glfw_callbacks();
    printf("Setting the keycallback\n");
}

void MandelWindow::set_mouse_motion_callback(
    MouseMotionCallback mouse_motion_callback
) {
    if (this->mouse_motion_callback) {
        printf("Existing Mouse Motion Callback is being replaced\n");
    }

    this->mouse_motion_callback = mouse_motion_callback;

    this->set_glfw_callbacks();
}

void MandelWindow::set_mouse_motion_delta_callback(
    MouseMotionDeltaCallback mouse_motion_delta_callback
) {
    if (this->mouse_motion_delta_callback) {
        printf("Existing Mouse Motion Delta Callback is being replaced\n");
    }

    this->mouse_motion_delta_callback = mouse_motion_delta_callback;

    this->set_glfw_callbacks();
}

void MandelWindow::set_mouse_scroll_callback(
    MouseScrollCallback mouse_scroll_callback
) {
    if (this->mouse_scroll_callback) {
        printf("Existing Mouse Scroll Callback is being replaced\n");
    }

    this->mouse_scroll_callback = mouse_scroll_callback;

    this->set_glfw_callbacks();
}

void MandelWindow::set_glfw_callbacks() {
    // Store 'this' pointer so static callback can access it
    glfwSetWindowUserPointer(this->window, this);

    // Set the static callback function
    glfwSetKeyCallback(this->window, MandelWindow::glfw_key_callback);
    glfwSetCursorPosCallback(this->window, MandelWindow::glfw_mouse_motion_callback);
    glfwSetScrollCallback(this->window, MandelWindow::glfw_scroll_motion_callback);
}

// static Callbacks
// TODO: Maybe change these to dynamic_cast?
void MandelWindow::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    auto user = static_cast<MandelWindow*>(glfwGetWindowUserPointer(window));

    if (user->key_callback) {
        user->key_callback(key, scancode, action, mode);
    }
}

void MandelWindow::glfw_mouse_motion_callback(GLFWwindow* window, double xpos, double ypos) {
    auto user = static_cast<MandelWindow*>(glfwGetWindowUserPointer(window));

    if (user->mouse_motion_callback) {
        user->mouse_motion_callback(xpos, ypos);
    }

    if (user->mouse_motion_delta_callback) {
        user->mouse_motion_delta_callback(xpos - user->prev_mouse_x, ypos - user->prev_mouse_y);
        user->prev_mouse_x = xpos;
        user->prev_mouse_y = ypos;
    }
}

void MandelWindow::glfw_scroll_motion_callback(GLFWwindow* window, double x_offset, double y_offset) {
    auto user = static_cast<MandelWindow*>(glfwGetWindowUserPointer(window));

    float new_zoom;
    user->renderer->modify_zoom([&](float& zoom) {
        zoom += (float)(y_offset * (zoom * 0.1));
        new_zoom = zoom;
    });

    user->renderer->modify_offset([&](float& offset) {
        offset -= (float)(x_offset * (new_zoom * 0.1));
    });

    if (user->mouse_scroll_callback) {
        user->mouse_scroll_callback(x_offset, y_offset);
    }
}
