#ifndef RENDER_PROPS_H
#define RENDER_PROPS_H

#include <webgpu/webgpu.hpp>

struct MandelDescriptor {
    float zoom{};
    float offset_x{};
    float offset_y{};
};

class RenderProps {
  public:
    RenderProps(wgpu::Device& device, wgpu::Queue& queue, wgpu::SurfaceConfiguration& config);
    RenderProps();
    ~RenderProps();

    void modify_zoom(std::function<void(float&)> function, wgpu::Queue& queue);
    void modify_offset(std::function<void(float&, float&)> function, wgpu::Queue& queue);
    void render(wgpu::RenderPassEncoder& render_pass);

  private:
    wgpu::RenderPipeline pipeline;
    wgpu::Buffer viewer_buffer;
    wgpu::BindGroupLayout bind_group_layout;
    wgpu::BindGroup bind_group;

    MandelDescriptor descriptor;
};

#endif
