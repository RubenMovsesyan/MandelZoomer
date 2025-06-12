#ifndef MANDEL_RENDER_H
#define MANDEL_RENDER_H

#include "render/RenderProps.h"
#include <utility>
#include <webgpu/webgpu.hpp>

class MandelRender {
  public:
    MandelRender(wgpu::Surface& surface, wgpu::Adapter& adapter, wgpu::Device& device, wgpu::Queue& queue);
    MandelRender();
    ~MandelRender();

    // Actually render something once the renderer has been initialized
    void modify_zoom(std::function<void(float&)> function);
    void modify_offset(std::function<void(float&, float&)> function);
    void render();

  private:
    wgpu::Surface* surface;
    wgpu::Device* device;
    wgpu::Queue* queue;

    RenderProps render_props;

    std::pair<WGPUSurfaceTexture, WGPUTextureView> get_next_surface_view_data();
};

#endif
