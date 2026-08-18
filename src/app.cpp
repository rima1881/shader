
#include <app.h>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <triangle.wgsl.h>
#include <webgpu/webgpu_cpp.h>


std::expected<void, AppCreateError> App::create(App& app, uint32_t width, uint32_t height, std::string title) {

    // initialize GLFW
    if (!glfwInit()) {
        return std::unexpected(AppCreateError::FailedToInitializeGLFW);
    }

    // configure window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // create window
    GLFWwindow* glfw_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!glfw_window) {
        glfwTerminate();
        return std::unexpected(AppCreateError::FailedToCreateWindow);
    }

    app.window = glfw_window;

    auto result = Renderer::create(app.renderer, app.window, width, height)
        .transform_error([](RendererCreateError err) {
            switch (err) {
                case RendererCreateError::FailedToCreateInstance: return AppCreateError::FailedToCreateInstance;
                case RendererCreateError::FailedToCreateSurface:  return AppCreateError::FailedToCreateSurface;
                case RendererCreateError::FailedToCreateAdapter:  return AppCreateError::FailedToCreateAdapter;
                case RendererCreateError::FailedToCreateDevice:   return AppCreateError::FailedToCreateDevice;
            }
            return AppCreateError::FailedToCreateInstance; // Fallback
        });

    if (!result) {
        glfwDestroyWindow(app.window);
        app.window = nullptr;
        glfwTerminate();
        return std::unexpected(result.error());
    }

    return {};
}

void App::run(App& app) {
    while (!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();

        Renderer::render(app.renderer);
    }
}

void App::destroy(App& app) {
    Renderer::destroy(app.renderer);
    glfwDestroyWindow(app.window);
    glfwTerminate();
}
