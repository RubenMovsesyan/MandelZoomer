#ifndef RENDER_PROPS_H
#define RENDER_PROPS_H

#include <webgpu/webgpu.hpp>

class RenderProps {
  public:
    RenderProps(wgpu::Device& device, wgpu::SurfaceConfiguration& config);
    RenderProps();
    ~RenderProps();

    void render(wgpu::RenderPassEncoder& render_pass);

  private:
    wgpu::RenderPipeline pipeline;
    wgpu::Buffer vertex_buffer;
};

#endif
