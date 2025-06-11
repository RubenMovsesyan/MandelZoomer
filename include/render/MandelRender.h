#ifndef MANDEL_RENDER_H
#define MANDEL_RENDER_H

#include <utility>
#include <webgpu/webgpu.h>

class MandelRender {
  public:
    MandelRender(WGPUSurface& surface, WGPUAdapter& adapter, WGPUDevice& device, WGPUQueue& queue);
    MandelRender();
    ~MandelRender();

    // Actually render something once the renderer has been initialized
    void render();

  private:
    WGPUSurface* surface;
    WGPUDevice* device;
    WGPUQueue* queue;

    std::pair<WGPUSurfaceTexture, WGPUTextureView> get_next_surface_view_data();
};

#endif
