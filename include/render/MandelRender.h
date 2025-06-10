#ifndef MANDEL_RENDER_H
#define MANDEL_RENDER_H

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>
#include <expected>
#include <string>

class MandelRender {
  public:
    MandelRender(SDL_Window* window);
    MandelRender();
    ~MandelRender();

    // Actually render something once the renderer has been initialized
    void render();

  private:
    // Function to load a shader
    std::expected<SDL_GPUShader*, std::string> LoadShader(
        const char* shader_file_name,
        Uint32 sampler_count,
        Uint32 uniform_buffer_count,
        Uint32 storage_buffer_count,
        Uint32 storage_texture_count
    );

    SDL_GPUDevice* device;
    SDL_GPUCommandBuffer* command_buffer;
    SDL_GPUTexture* swapchain_texture;

    // Pointer to the window for rendering
    SDL_Window* window;
};

#endif
