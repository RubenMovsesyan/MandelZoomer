#include "render/RenderProps.h"
#include "webgpu/webgpu.hpp"
#include <webgpu.h>

RenderProps::RenderProps(wgpu::Device& device, wgpu::Queue& queue, wgpu::SurfaceConfiguration& config) {
    // Initialize the descriptor with default values
    this->descriptor = MandelDescriptor{
        .zoom = 1.0,
    };

    wgpu::RenderPipelineDescriptor pipeline_desc;
    pipeline_desc.vertex.bufferCount = 0;
    pipeline_desc.vertex.buffers = nullptr;

    // Create the viewer buffer
    wgpu::BufferDescriptor buffer_desc;
    buffer_desc.label = wgpu::StringView("Viewer Buffer");
    buffer_desc.mappedAtCreation = false;
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst;
    buffer_desc.size = sizeof(MandelDescriptor);
    this->viewer_buffer = device.createBuffer(buffer_desc);

    std::vector<std::byte> bytes(sizeof this->descriptor);
    std::memcpy(bytes.data(), reinterpret_cast<void*>(&this->descriptor), sizeof this->descriptor);
    queue.writeBuffer(this->viewer_buffer, 0, bytes.data(), bytes.size());

    // Create the layout for this bind group
    wgpu::BindGroupLayoutDescriptor bind_group_layout_desc;
    bind_group_layout_desc.label = wgpu::StringView("Viewer Bind Group Layout");
    bind_group_layout_desc.entryCount = 1;

    // Define the entries for the bind group layout
    wgpu::BindGroupLayoutEntry layout_entry;
    layout_entry.binding = 0;
    wgpu::BufferBindingLayout buffer_binding_layout;
    buffer_binding_layout.nextInChain = nullptr;
    buffer_binding_layout.hasDynamicOffset = false;
    buffer_binding_layout.minBindingSize = 0;
    buffer_binding_layout.type = wgpu::BufferBindingType::Uniform;
    layout_entry.nextInChain = nullptr;
    layout_entry.buffer = buffer_binding_layout;
    layout_entry.visibility = wgpu::ShaderStage::Fragment;
    std::array<wgpu::BindGroupLayoutEntry, 1> entries{layout_entry};
    bind_group_layout_desc.entries = entries.data();
    this->bind_group_layout = device.createBindGroupLayout(bind_group_layout_desc);

    // Create the bind group
    wgpu::BindGroupDescriptor bind_group_descriptor;
    bind_group_descriptor.label = wgpu::StringView("Viewer Bind Group");
    bind_group_descriptor.entryCount = 1;
    bind_group_descriptor.layout = this->bind_group_layout;

    // Create the entries for the bind group
    wgpu::BindGroupEntry bind_group_entry;
    bind_group_entry.binding = 0;
    bind_group_entry.buffer = this->viewer_buffer;
    bind_group_entry.offset = 0;
    bind_group_entry.size = sizeof(MandelDescriptor);
    bind_group_entry.nextInChain = nullptr;
    std::array<wgpu::BindGroupEntry, 1> bind_group_entries{bind_group_entry};
    bind_group_descriptor.entryCount = 1;
    bind_group_descriptor.entries = bind_group_entries.data();
    this->bind_group = device.createBindGroup(bind_group_descriptor);

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

    struct Descriptor {
    	zoom: f32,
     	offset_x: f32,
      	offset_y: f32,
    }

    @group(0) @binding(0)
    var<uniform> descriptor: Descriptor;

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
    	var c: vec2<f32> = vertex_input.uv * descriptor.zoom + vec2<f32>(descriptor.offset_x, descriptor.offset_y);

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

    // Create a pipeline layout based on the bind group
    wgpu::PipelineLayoutDescriptor pipeline_layout_desc;
    pipeline_layout_desc.label = wgpu::StringView("Mandel Pipeline Layout");
    pipeline_layout_desc.nextInChain = nullptr;
    // For some reasone I can't use the hpp header for this one
    std::array<WGPUBindGroupLayout, 1> bind_group_layouts{this->bind_group_layout};
    pipeline_layout_desc.bindGroupLayoutCount = bind_group_layouts.size();
    pipeline_layout_desc.bindGroupLayouts = bind_group_layouts.data();
    wgpu::PipelineLayout pipeline_layout = device.createPipelineLayout(pipeline_layout_desc);
    pipeline_desc.layout = pipeline_layout;

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

void RenderProps::modify_zoom(std::function<void(float&)> callback, wgpu::Queue& queue) {
    callback(this->descriptor.zoom);

    std::vector<std::byte> bytes(sizeof this->descriptor);
    std::memcpy(bytes.data(), reinterpret_cast<void*>(&this->descriptor), sizeof this->descriptor);
    queue.writeBuffer(this->viewer_buffer, 0, bytes.data(), bytes.size());
}

void RenderProps::modify_offset(std::function<void(float&, float&)> callback, wgpu::Queue& queue) {
    callback(this->descriptor.offset_x, this->descriptor.offset_y);

    std::vector<std::byte> bytes(sizeof this->descriptor);
    std::memcpy(bytes.data(), reinterpret_cast<void*>(&this->descriptor), sizeof this->descriptor);
    queue.writeBuffer(this->viewer_buffer, 0, bytes.data(), bytes.size());
}

void RenderProps::render(wgpu::RenderPassEncoder& render_pass) {
    render_pass.setPipeline(this->pipeline);

    // Set the bind group for the descriptor
    render_pass.setBindGroup(0, this->bind_group, 0, {});

    render_pass.draw(6, 1, 0, 0);
}
