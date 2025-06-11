#include "render/RenderProps.h"
#include "webgpu/webgpu.hpp"

RenderProps::RenderProps(wgpu::Device& device) {
    wgpu::RenderPipelineDescriptor pipeline_desc;
    pipeline_desc.vertex.bufferCount = 0;
    pipeline_desc.vertex.buffers = nullptr;

    // Define the shader module
    wgpu::ShaderModuleDescriptor vert_shader_desc;
    vert_shader_desc.label = wgpu::StringView("Vertex Shader");

    wgpu::ShaderModuleDescriptor frag_shader_desc;
    frag_shader_desc.label = wgpu::StringView("Fragment Shader");

    const char* vert_shader_code = R"(
    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) uv: vec2<f32>,
    }

    @vertex
    fn main(
        @builtin(vertex_index) vertex_id: u32
    ) -> VertexOutput {
        let uv: vec2<f32> = vec2<f32>(f32((vertex_id << 1) & 2), f32(vertex_id & 2));
        let pos = uv * 2.0 - vec2<f32>(1.0, 1.0);

        var output: VertexOutput;
        output.position = vec4<f32>(pos, 0.0, 1.0);
        output.uv = uv;

        return output;
    }
    )";

    const char* frag_shader_code = R"(
    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) uv: vec2<f32>,
    }

    @fragment
    fn main(vertex_input: VertexOutput) -> @location(0) vec4<f32> {
        return vec4<f32>(vertex_input.uv, 0.0, 1.0);
    }
    )";

    wgpu::ShaderSourceWGSL vert_shader_source;
    vert_shader_source.chain.next = nullptr;
    vert_shader_source.chain.sType = wgpu::SType::ShaderSourceWGSL; // Tutorial is outdated with deprecated types
    vert_shader_source.code = wgpu::StringView(vert_shader_code);
    vert_shader_desc.nextInChain = &vert_shader_source.chain;
    wgpu::ShaderModule vert_shader_module = device.createShaderModule(vert_shader_desc);

    wgpu::ShaderSourceWGSL frag_shader_source;
    frag_shader_source.chain.next = nullptr;
    frag_shader_source.chain.sType = wgpu::SType::ShaderSourceWGSL;
    frag_shader_source.code = wgpu::StringView(frag_shader_code);
    frag_shader_desc.nextInChain = &frag_shader_source.chain;
    wgpu::ShaderModule frag_shader_module = device.createShaderModule(frag_shader_desc);

    // this->pipeline = device.createRenderPipeline(pipeline_desc);

    vert_shader_module.release();
    frag_shader_module.release();
}

RenderProps::RenderProps() {}

RenderProps::~RenderProps() {
}
