#include "render/MandelRender.h"
#include "exception/FileIOException.h"
#include "exception/SDLException.h"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <vector>

// Temp
const char* SHADER_FILE_NAME = "mandel.frag";

MandelRender::MandelRender(SDL_Window* window) {
    // Acquire the window pointer for ourselves
    this->window = window;

    this->device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV |   // Vulkan
            SDL_GPU_SHADERFORMAT_MSL | // Metal
            SDL_GPU_SHADERFORMAT_DXIL, // DirectX
        true,                          // Debug mode
        nullptr                        // Prefered GPU driver (null to make sdl choose)
    );

    if (!this->device) {
        throw SDLException("Couldn't Create GPU device");
    }

    // Claim the window for the gpu device
    if (!SDL_ClaimWindowForGPUDevice(this->device, this->window)) {
        throw SDLException("Couldn't Claim window for the GPU device");
    }

    std::expected<SDL_GPUShader*, std::string> mandel_shader = this->LoadShader(
        SHADER_FILE_NAME,
        0,
        0,
        0,
        0
    );

    if (mandel_shader.has_value()) {
    } else {
        throw FileIOException(mandel_shader.error());
    }
}

MandelRender::MandelRender() {}

MandelRender::~MandelRender() {}

void MandelRender::render() {
    // Acquire the GPU command buffer (equivalent of queue in WGPU)
    this->command_buffer = SDL_AcquireGPUCommandBuffer(this->device);
    if (!this->command_buffer) {
        throw SDLException("Couldn't Acquire GPU Command Buffer from GPU Device");
    }

    // Acquire the GPU swapchain texture for attaching to the renderer
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            this->command_buffer,
            this->window,
            &this->swapchain_texture,
            nullptr,
            nullptr
        )) {
        throw SDLException("Couldn't Acquire GPU Swapchain Texture");
    }

    // Temp
    SDL_GPUColorTargetInfo color_target{};
    color_target.texture = swapchain_texture;
    color_target.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target.store_op = SDL_GPU_STOREOP_STORE;
    color_target.clear_color = SDL_FColor{1.0f, 0.0f, 1.0f, 1.0f};

    std::vector<SDL_GPUColorTargetInfo> color_targets{color_target};

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        this->command_buffer,
        color_targets.data(),
        color_targets.size(),
        nullptr // Don't need depth becuse this is a 2d application
    );

    // What starts must always end
    SDL_EndGPURenderPass(render_pass);

    // Submit the command buffer to the gpu
    if (!SDL_SubmitGPUCommandBuffer(this->command_buffer)) {
        throw SDLException("Couldn't Submit GPU command buffer");
    }
}

// ======================= Private Implementation =======================

std::expected<SDL_GPUShader*, std::string> MandelRender::LoadShader(
    const char* shader_file_name,
    Uint32 sampler_count,
    Uint32 uniform_buffer_count,
    Uint32 storage_buffer_count,
    Uint32 storage_texture_count
) {
    const char* base_path = SDL_GetBasePath();

    // Auto detect the shader stage from the file name
    SDL_GPUShaderStage stage;
    if (SDL_strstr(shader_file_name, ".vert")) {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    } else if (SDL_strstr(shader_file_name, ".frag")) {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    } else {
        return std::unexpected("Could Not Detect Shader Type");
    }

    char full_path[256];
    SDL_GPUShaderFormat backend_formats = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
    const char* entry_point;

    if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
        SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/SPIRV/%s.spv", base_path, shader_file_name);
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entry_point = "main";
    } else if (backend_formats & SDL_GPU_SHADERFORMAT_MSL) {
        SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/MSL/%s.msl", base_path, shader_file_name);
        format = SDL_GPU_SHADERFORMAT_MSL;
        entry_point = "main0";
    } else if (backend_formats & SDL_GPU_SHADERFORMAT_DXIL) {
        SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/DXIL/%s.dxil", base_path, shader_file_name);
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entry_point = "main";
    } else {
        return std::unexpected("Unrecognized backend shader format!");
    }

    size_t code_size;
    void* code = SDL_LoadFile(full_path, &code_size);

    if (!code) {
        return std::unexpected(std::format("Failed to load Shader from disk! {}", full_path));
    }

    SDL_GPUShaderCreateInfo shader_info = {
        .code = (uint8_t*)code,
        .code_size = code_size,
        .entrypoint = entry_point,
        .format = format,
        .stage = stage,
        .num_samplers = sampler_count,
        .num_uniform_buffers = uniform_buffer_count,
        .num_storage_buffers = storage_buffer_count,
        .num_storage_textures = storage_texture_count
    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(this->device, &shader_info);

    // Free the code pointer after its been used
    SDL_free(code);
    if (!shader) {
        return std::unexpected("Failed to Create Shader!");
    }

    return shader;
}
