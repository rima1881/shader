
#ifndef APP_H
#define APP_H
#include <GLFW/glfw3.h>
#include <expected>
#include <string>
#include <renderer.h>
#include <ui/ui.h>
#include <backend/backend.h>

enum class AppCreateError {
    FailedToInitializeGLFW,
    FailedToCreateWindow,
    FailedToCreateInstance,
    FailedToCreateSurface,
    FailedToCreateAdapter,
    FailedToCreateDevice,
};

struct App {

    Backend backend;
    Renderer renderer;
    UI ui;
    GLFWwindow* window = nullptr;

    static std::expected<void, AppCreateError> create(App& app, uint32_t width, uint32_t height, std::string title);
    static void run(App& app);
    static void destroy(App& app);
};

#endif
