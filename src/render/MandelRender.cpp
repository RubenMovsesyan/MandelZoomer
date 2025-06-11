// #include "render/MandelRender.h"
// #include "exception/FileIOException.h"
// #include "exception/SDLException.h"
// #include "render/RenderSurface.hpp"
// #include <SDL3/SDL_error.h>
// #include <SDL3/SDL_filesystem.h>
// #include <SDL3/SDL_gpu.h>
// #include <SDL3/SDL_pixels.h>
// #include <SDL3/SDL_render.h>

// // Temp
// const char* VERT_FILE_NAME = "mandel.vert";
// const char* FRAG_FILE_NAME = "mandel.frag";

// MandelRender::MandelRender(SDL_Window* window) {
//     // Acquire the window pointer for ourselves
//     this->window = window;

//     this->device = SDL_CreateGPUDevice(
//         SDL_GPU_SHADERFORMAT_SPIRV |   // Vulkan
//             SDL_GPU_SHADERFORMAT_MSL | // Metal
//             SDL_GPU_SHADERFORMAT_DXIL, // DirectX
//         true,                          // Debug mode
//         nullptr                        // Prefered GPU driver (null to make sdl choose)
//     );

//     if (!this->device) {
//         throw SDLException("Couldn't Create GPU device");
//     }

//     // Claim the window for the gpu device
//     if (!SDL_ClaimWindowForGPUDevice(this->device, this->window)) {
//         throw SDLException("Couldn't Claim window for the GPU device");
//     }

//     // Create the vertex and fragment shader
//     auto mandel_vertex_shader = this->LoadShader(VERT_FILE_NAME, 0, 0, 0, 0);
//     if (!mandel_vertex_shader) {
//         throw FileIOException(mandel_vertex_shader.error());
//     }

//     auto mandel_fragment_shader = this->LoadShader(FRAG_FILE_NAME, 0, 0, 0, 0);
//     if (!mandel_fragment_shader) {
//         throw FileIOException(mandel_fragment_shader.error());
//     }

//     // Set up the color targets
//     SDL_GPUColorTargetInfo color_target = {
//         .texture = nullptr,
//         .load_op = SDL_GPU_LOADOP_CLEAR,
//         .store_op = SDL_GPU_STOREOP_STORE,
//         .clear_color = SDL_FColor{1.0f, 0.0f, 1.0f, 1.0f},
//     };

//     this->color_targets[0] = color_target;

//     SDL_GPUColorTargetDescription color_target_description = {
//         .format = SDL_GetGPUSwapchainTextureFormat(this->device, this->window),
//     };

//     this->color_target_descriptions[0] = color_target_description;

//     // Create the grapchis pipeline with a shader
//     SDL_GPUVertexBufferDescription buffer_descriptoins[] = {
//         {.slot = 0,
//          .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
//          .instance_step_rate = 0,
//          .pitch = sizeof(float) * 2
//         }
//     };
//     SDL_GPUVertexAttribute vertex_attributes[] = {
//         {.buffer_slot = 0,
//          .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
//          .location = 0,
//          .offset = 0
//         }
//     };

//     SDL_GPUVertexInputState vertex_input_state = {
//         .num_vertex_buffers = 0,
//         .vertex_buffer_descriptions = nullptr,
//         .num_vertex_attributes = 0,
//         .vertex_attributes = nullptr,
//     };

//     SDL_GPUGraphicsPipelineCreateInfo create_info = {
//         .vertex_shader = mandel_vertex_shader.value(),
//         .fragment_shader = mandel_fragment_shader.value(),
//         .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
//         .rasterizer_state = {
//             .fill_mode = SDL_GPU_FILLMODE_FILL,
//             .cull_mode = SDL_GPU_CULLMODE_NONE,
//             .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
//             .depth_bias_constant_factor = 0.0f,
//             .depth_bias_clamp = 0.0f,
//             .depth_bias_slope_factor = 0.0f,
//             .enable_depth_bias = false,
//         },
//         .target_info = {
//             .num_color_targets = COLOR_TARGET_DESCRIPTION_LEN,
//             .color_target_descriptions = this->color_target_descriptions,
//         },
//         .vertex_input_state = vertex_input_state,
//     };

//     this->pipeline = SDL_CreateGPUGraphicsPipeline(
//         this->device,
//         &create_info
//     );

//     if (!this->pipeline) {
//         throw SDLException("Failed to create Pipeline");
//     }

//     SDL_ReleaseGPUShader(this->device, mandel_vertex_shader.value());
//     SDL_ReleaseGPUShader(this->device, mandel_fragment_shader.value());

//     // Create the vertex buffer
//     // SDL_GPUBufferCreateInfo vertex_create_info = {
//     //     .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
//     //     .size = sizeof(float) * 2 * 6
//     // };

//     // this->vertex_buffer = SDL_CreateGPUBuffer(
//     //     this->device,
//     //     &vertex_create_info
//     // );

//     // if (!this->vertex_buffer) {
//     //     throw SDLException("Could not Create Vertex Buffer");
//     // }

//     // // Send the data to the gpu with a transfer buffer
//     // SDL_GPUTransferBufferCreateInfo transfer_create_info = {
//     //     .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
//     //     .size = sizeof(float) * 2 * 6
//     // };

//     // SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(
//     //     this->device,
//     //     &transfer_create_info
//     // );

//     // if (!transfer_buffer) {
//     //     throw SDLException("Could Not Create Transfer Buffer");
//     // }

//     // float* transfer_data = (float*)SDL_MapGPUTransferBuffer(
//     //     this->device,
//     //     transfer_buffer,
//     //     false
//     // );

//     // for (int i = 0; i < 12; i++) {
//     //     transfer_data[i] = 1.0;
//     // }

//     // // Unmap the transfer buffer once the data has been written
//     // SDL_UnmapGPUTransferBuffer(this->device, transfer_buffer);

//     // // Upload the transfer data to the vertex buffer
//     // SDL_GPUCommandBuffer* upload_command_bufer = SDL_AcquireGPUCommandBuffer(this->device);
//     // if (!upload_command_bufer) {
//     //     throw SDLException("Could not acquire gpu command buffer for uploading vertex data");
//     // }

//     // {
//     //     SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_command_bufer);

//     //     SDL_GPUTransferBufferLocation location = {
//     //         .transfer_buffer = transfer_buffer,
//     //         .offset = 0
//     //     };

//     //     SDL_GPUBufferRegion region = {
//     //         .buffer = this->vertex_buffer,
//     //         .offset = 0,
//     //         .size = sizeof(float) * 2 * 6
//     //     };

//     //     SDL_UploadToGPUBuffer(
//     //         copy_pass,
//     //         &location,
//     //         &region,
//     //         false
//     //     );

//     //     SDL_EndGPUCopyPass(copy_pass);
//     // }

//     // if (!SDL_SubmitGPUCommandBuffer(upload_command_bufer)) {
//     //     throw SDLException("Failed to submit upload command buffer to GPU");
//     // }

//     // SDL_ReleaseGPUTransferBuffer(this->device, transfer_buffer);
// }

// MandelRender::MandelRender() {}

// MandelRender::~MandelRender() {
//     SDL_ReleaseGPUGraphicsPipeline(this->device, this->pipeline);
//     // SDL_ReleaseGPUBuffer(this->device, this->vertex_buffer);
// }

// void MandelRender::render() {
//     // Acquire the GPU command buffer (equivalent of queue in WGPU)
//     this->command_buffer = SDL_AcquireGPUCommandBuffer(this->device);
//     if (!this->command_buffer) {
//         throw SDLException("Couldn't Acquire GPU Command Buffer from GPU Device");
//     }

//     // Acquire the GPU swapchain texture for attaching to the renderer
//     if (!SDL_WaitAndAcquireGPUSwapchainTexture(
//             this->command_buffer,
//             this->window,
//             &this->swapchain_texture,
//             nullptr,
//             nullptr
//         )) {
//         throw SDLException("Couldn't Acquire GPU Swapchain Texture");
//     }

//     this->color_targets[0].texture = this->swapchain_texture;

//     {
//         // Perform the render pass
//         SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
//             this->command_buffer,
//             this->color_targets,
//             COLOR_TARGETS_LEN,
//             nullptr // Don't need depth becuse this is a 2d application
//         );

//         // Create the vetex buffer binding
//         // SDL_GPUBufferBinding vertex_binding = {
//         //     .buffer = this->vertex_buffer,
//         //     .offset = 0,
//         // };

//         printf("About to bind pipeline...\n");
//         SDL_BindGPUGraphicsPipeline(render_pass, this->pipeline);
//         printf("Pipeline bound!\n");
//         // SDL_BindGPUVertexBuffers(
//         //     render_pass,
//         //     0, // First Slot
//         //     &vertex_binding,
//         //     1 // Num Bindings
//         // );
//         // SDL_DrawGPUPrimitives(
//         //     render_pass,
//         //     3,
//         //     1, // Num Instances
//         //     0, // First Vertex
//         //     0  // First Instance
//         // );
//         // What starts must always end
//         SDL_EndGPURenderPass(render_pass);
//     }

//     // Submit the command buffer to the gpu
//     if (!SDL_SubmitGPUCommandBuffer(this->command_buffer)) {
//         throw SDLException("Couldn't Submit GPU command buffer");
//     }
// }

// // ======================= Private Implementation =======================

// std::expected<SDL_GPUShader*, std::string> MandelRender::LoadShader(
//     const char* shader_file_name,
//     Uint32 sampler_count,
//     Uint32 uniform_buffer_count,
//     Uint32 storage_buffer_count,
//     Uint32 storage_texture_count
// ) {
//     const char* base_path = SDL_GetBasePath();

//     // Auto detect the shader stage from the file name
//     SDL_GPUShaderStage stage;
//     if (SDL_strstr(shader_file_name, ".vert")) {
//         stage = SDL_GPU_SHADERSTAGE_VERTEX;
//     } else if (SDL_strstr(shader_file_name, ".frag")) {
//         stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
//     } else {
//         return std::unexpected("Could Not Detect Shader Type");
//     }

//     char full_path[256];
//     SDL_GPUShaderFormat backend_formats = SDL_GetGPUShaderFormats(this->device);
//     SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
//     const char* entry_point;

//     if (backend_formats & SDL_GPU_SHADERFORMAT_SPIRV) {
//         SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/SPIRV/%s.spv", base_path, shader_file_name);
//         format = SDL_GPU_SHADERFORMAT_SPIRV;
//         entry_point = "main";
//     } else if (backend_formats & SDL_GPU_SHADERFORMAT_MSL) {
//         SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/MSL/%s.msl", base_path, shader_file_name);
//         format = SDL_GPU_SHADERFORMAT_MSL;
//         entry_point = "main0";
//     } else if (backend_formats & SDL_GPU_SHADERFORMAT_DXIL) {
//         SDL_snprintf(full_path, sizeof(full_path), "%s../Content/Shaders/Compiled/DXIL/%s.dxil", base_path, shader_file_name);
//         format = SDL_GPU_SHADERFORMAT_DXIL;
//         entry_point = "main";
//     } else {
//         return std::unexpected("Unrecognized backend shader format!");
//     }

//     size_t code_size;
//     void* code = SDL_LoadFile(full_path, &code_size);

//     if (!code) {
//         return std::unexpected(std::format("Failed to load Shader from disk! {}", full_path));
//     }

//     SDL_GPUShaderCreateInfo shader_info = {
//         .code = (uint8_t*)code,
//         .code_size = code_size,
//         .entrypoint = entry_point,
//         .format = format,
//         .stage = stage,
//         .num_samplers = sampler_count,
//         .num_uniform_buffers = uniform_buffer_count,
//         .num_storage_buffers = storage_buffer_count,
//         .num_storage_textures = storage_texture_count
//     };

//     SDL_GPUShader* shader = SDL_CreateGPUShader(this->device, &shader_info);

//     // Free the code pointer after its been used
//     SDL_free(code);
//     if (!shader) {
//         return std::unexpected(std::format("Failed to Create Shader! {}", SDL_GetError()));
//     }

//     return shader;
// }
