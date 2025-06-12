#include "render/RenderProps.h"
#include "webgpu/webgpu.hpp"

RenderProps::RenderProps(wgpu::Device& device, wgpu::SurfaceConfiguration& config) {
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
        // let uv: vec2<f32> = vec2<f32>(f32((vertex_id << 1) & 2), f32(vertex_id & 2));
        // let pos = uv * 2.0 - vec2<f32>(1.0, 1.0);

        var output: VertexOutput;
        var pos: vec2<f32>;
        // output.position = vec4<f32>(pos, 0.0, 1.0);
        // output.uv = uv;
        switch (vertex_id) {
        	case 0u: {
                pos.x = -1.0;
                pos.y = -1.0;
                break;
            }
            case 1u: {
                pos.x = -1.0;
                pos.y = 1.0;
                break;
            }
            case 2u: {
                pos.x = 1.0;
                pos.y = -1.0;
                break;
            }
            case 3u: {
                pos.x = 1.0;
                pos.y = -1.0;
                break;
            }
            case 4u: {
                pos.x = -1.0;
                pos.y = 1.0;
                break;
            }
            case 5u: {
                pos.x = 1.0;
                pos.y = 1.0;
                break;
            }
            default: {
	            pos.x = 1.0;
	            pos.y = -1.0;
	           	break;
            }
        }

        output.position = vec4<f32>(pos, 0.0, 1.0);
        output.uv = pos;
        return output;
    }
    )";

    const char* frag_shader_code = R"(
    struct VertexOutput {
        @builtin(position) position: vec4<f32>,
        @location(0) uv: vec2<f32>,
    }

    fn complex_mult(complex1: vec2<f32>, complex2: vec2<f32>) -> vec2<f32> {
    	let a = complex1.x;
     	let b = complex1.y;

     	let c = complex2.x;
      	let d = complex2.y;

      	let real = (a * c) - (b * d);
      	let imag = (a * d) + (b * c);

       	return vec2<f32>(real, imag);
    }

    @fragment
    fn main(vertex_input: VertexOutput) -> @location(0) vec4<f32> {
    	var c: vec2<f32> = vertex_input.uv;

     	var z: vec2<f32> = vec2<f32>(0.0, 0.0);

      	for (var i = 0; i < 32; i++) {
       		z = complex_mult(z, z) + c;
         	if (length(z) >= 2.0) {
          		return vec4<f32>(f32(i) / 32.0, 0.0, f32(i) / 32.0, 1.0);
            }
       	}

    	return vec4<f32>(0.0, 0.0, 0.0, 1.0);
    }
    )";

    wgpu::ShaderSourceWGSL vert_shader_source;
    vert_shader_source.chain.next = nullptr;
    vert_shader_source.chain.sType = wgpu::SType::ShaderSourceWGSL; // Tutorial is outdated with deprecated types
    vert_shader_source.code = wgpu::StringView(vert_shader_code);
    vert_shader_desc.nextInChain = &vert_shader_source.chain;
    wgpu::ShaderModule vert_shader_module = device.createShaderModule(vert_shader_desc);

    wgpu::VertexState vertex_state;
    vertex_state.module = vert_shader_module;
    vertex_state.entryPoint = wgpu::StringView("main");
    pipeline_desc.vertex = vertex_state;

    wgpu::ShaderSourceWGSL frag_shader_source;
    frag_shader_source.chain.next = nullptr;
    frag_shader_source.chain.sType = wgpu::SType::ShaderSourceWGSL;
    frag_shader_source.code = wgpu::StringView(frag_shader_code);
    frag_shader_desc.nextInChain = &frag_shader_source.chain;
    wgpu::ShaderModule frag_shader_module = device.createShaderModule(frag_shader_desc);

    wgpu::BlendState blend_state;
    blend_state.setDefault();

    wgpu::ColorTargetState color_target_state;
    color_target_state.nextInChain = nullptr;
    color_target_state.format = config.format;
    color_target_state.blend = &blend_state;
    color_target_state.writeMask = wgpu::ColorWriteMask::All;

    wgpu::FragmentState fragment_state;
    fragment_state.module = frag_shader_module;
    std::array<wgpu::ColorTargetState, 1> targets = {{color_target_state}};
    fragment_state.targetCount = targets.size();
    fragment_state.targets = targets.data();
    fragment_state.entryPoint = wgpu::StringView("main");
    pipeline_desc.fragment = &fragment_state;

    wgpu::PrimitiveState primitive_state;
    primitive_state.cullMode = wgpu::CullMode::None;
    primitive_state.frontFace = wgpu::FrontFace::CCW;
    primitive_state.topology = wgpu::PrimitiveTopology::TriangleList;
    primitive_state.unclippedDepth = false;
    primitive_state.nextInChain = nullptr;
    pipeline_desc.primitive = primitive_state;

    pipeline_desc.depthStencil = nullptr;

    wgpu::MultisampleState multisample_state;
    multisample_state.count = 1;
    multisample_state.mask = !0;
    multisample_state.alphaToCoverageEnabled = false;
    pipeline_desc.multisample = multisample_state;

    this->pipeline = device.createRenderPipeline(pipeline_desc);

    vert_shader_module.release();
    frag_shader_module.release();
}

RenderProps::RenderProps() {}

RenderProps::~RenderProps() {
}

void RenderProps::render(wgpu::RenderPassEncoder& render_pass) {
    render_pass.setPipeline(this->pipeline);
    render_pass.draw(6, 1, 0, 0);
}
