#ifndef RENDER_PROPS_H
#define RENDER_PROPS_H

#include <webgpu/webgpu.hpp>

class RenderProps {
  public:
    RenderProps(wgpu::Device& device);
    RenderProps();
    ~RenderProps();

  private:
    wgpu::RenderPipeline pipeline;
    wgpu::Buffer vertex_buffer;
};

#endif
