
#ifndef RENDERER_H
#define RENDERER_H

#include <webgpu/webgpu_cpp.h>
#include <expected>
#include <GLFW/glfw3.h>
#include <triangle.wgsl.h>
#include <ui/ui.h>


#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

#ifdef Success
#undef Success
#endif
#ifdef None
#undef None
#endif

enum class RendererCreateError {
    FailedToCreateInstance,
    FailedToCreateSurface,
    FailedToCreateAdapter,
    FailedToCreateDevice,
};


struct Renderer {
    wgpu::Instance instance;
    wgpu::Adapter adapter;
    wgpu::Device device;
    wgpu::Surface surface;
    wgpu::TextureFormat format;
    wgpu::RenderPipeline pipeline;
    wgpu::Queue queue;
    wgpu::Buffer vertexBuffer;
    wgpu::Buffer indexBuffer;

    // TODO: see if i can remove width/height
    static std::expected<void, RendererCreateError> create(Renderer& renderer, GLFWwindow* window, const UI& ui, uint32_t width, uint32_t height);
    static void render(Renderer& renderer, const UI& ui);
    static void destroy(Renderer& renderer);
};

#endif
