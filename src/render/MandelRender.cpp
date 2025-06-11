#include "render/MandelRender.h"
#include "exception/WGPUException.h"
#include <format>
#include <stdio.h>
#include <webgpu/webgpu.h>

MandelRender::MandelRender(WGPUSurface& surface, WGPUAdapter& adapter, WGPUDevice& device, WGPUQueue& queue) {
    this->device = &device;
    this->surface = &surface;
    this->queue = &queue;

    WGPUSurfaceConfiguration surface_config = {};

    surface_config.nextInChain = nullptr;
    surface_config.width = 1280;
    surface_config.height = 720;

    // WGPUTextureFormat surface_format = wgpuSurfaceGetPreferredFormat(*surface, *adapter);
    WGPUSurfaceCapabilities capabilities{
        .nextInChain = nullptr,
    };

    wgpuSurfaceGetCapabilities(*this->surface, adapter, &capabilities);
    printf("Got Sufrace Capabilities\n");

    // Temp TODO: get the preferred format
    surface_config.format = capabilities.formats[0];
    surface_config.viewFormatCount = 0;
    surface_config.viewFormats = nullptr;
    surface_config.usage = WGPUTextureUsage_RenderAttachment;
    surface_config.device = *this->device;
    surface_config.presentMode = WGPUPresentMode_Fifo;
    surface_config.alphaMode = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(*this->surface, &surface_config);
    printf("Configured Surface\n");
}

MandelRender::MandelRender() {}

MandelRender::~MandelRender() {
    wgpuSurfaceUnconfigure(*this->surface);
}

void MandelRender::render() {
    auto [surface_texture, texture_view] = this->get_next_surface_view_data();
    if (!texture_view) {
        throw WGPUException(std::format("Could Not Get Surface View"));
    }

    WGPUCommandEncoderDescriptor command_encoder_descriptor{
        .nextInChain = nullptr,
        .label = "Command Encoder",
    };

    WGPUCommandEncoder command_encoder = wgpuDeviceCreateCommandEncoder(*this->device, &command_encoder_descriptor);

    WGPURenderPassColorAttachment render_pass_color_attachment = {
        .nextInChain = nullptr,
        .view = texture_view,
        .resolveTarget = nullptr,
        .loadOp = WGPULoadOp_Clear,
        .storeOp = WGPUStoreOp_Store,
        .clearValue = WGPUColor{1.0, 0.0, 1.0, 1.0},
        .depthSlice = WGPU_DEPTH_SLICE_UNDEFINED,
    };

    WGPURenderPassDescriptor render_pass_desc = {
        .nextInChain = nullptr,
        .colorAttachmentCount = 1,
        .colorAttachments = &render_pass_color_attachment,
        .depthStencilAttachment = nullptr,
        .timestampWrites = nullptr,
    };

    // Create the render pass
    WGPURenderPassEncoder render_pass = wgpuCommandEncoderBeginRenderPass(command_encoder, &render_pass_desc);
    /*
     *
     *  DO STUFF HERE
     *
     */

    // End the render pass
    wgpuRenderPassEncoderEnd(render_pass);
    wgpuRenderPassEncoderRelease(render_pass);

    // Encode and submit the render pass
    WGPUCommandBufferDescriptor command_buffer_descriptor{
        .nextInChain = nullptr,
        .label = "Command Buffer",
    };
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(command_encoder, &command_buffer_descriptor);
    wgpuCommandEncoderRelease(command_encoder);

    // Submit the queue at the end of the render
    wgpuQueueSubmit(*this->queue, 1, &command);
    wgpuCommandBufferRelease(command);

    wgpuTextureViewRelease(texture_view);

    // Actually display the result
    wgpuSurfacePresent(*this->surface);
}

std::pair<WGPUSurfaceTexture, WGPUTextureView> MandelRender::get_next_surface_view_data() {
    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(*this->surface, &surface_texture);

    if (
        !(surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal |
          surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
    ) {
        return {surface_texture, nullptr};
    }

    WGPUTextureViewDescriptor view_descriptor{
        .nextInChain = nullptr,
        .label = "Surface texture view",
        .format = wgpuTextureGetFormat(surface_texture.texture),
        .dimension = WGPUTextureViewDimension_2D,
        .baseMipLevel = 0,
        .mipLevelCount = 1,
        .baseArrayLayer = 0,
        .arrayLayerCount = 1,
        .aspect = WGPUTextureAspect_All,
    };

    WGPUTextureView target_view = wgpuTextureCreateView(surface_texture.texture, &view_descriptor);

    return {surface_texture, target_view};
}
