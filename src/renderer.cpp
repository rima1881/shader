
#include <renderer.h>

std::expected<void, RendererCreateError> Renderer::create(Renderer& renderer, GLFWwindow* window, uint32_t width, uint32_t height) {
    wgpu::InstanceFeatureName timedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;

    wgpu::InstanceDescriptor instanceDesc{
        .requiredFeatureCount = 1,
        .requiredFeatures = &timedWaitAny,
    };
    renderer.instance = wgpu::CreateInstance(&instanceDesc);
    if (!renderer.instance) {
        return std::unexpected(RendererCreateError::FailedToCreateInstance);
    }

    wgpu::SurfaceDescriptor surfaceDesc = {};
    #if defined(_WIN32)
        wgpu::SurfaceSourceWindowsHWND hwndDesc = {};
        hwndDesc.hwnd = glfwGetWin32Window(window);
        hwndDesc.hinstance = GetModuleHandle(nullptr);
        surfaceDesc.nextInChain = &hwndDesc;
    #elif defined(__APPLE__)
        wgpu::SurfaceSourceMetalLayer metalLayerDesc = {};
        surfaceDesc.nextInChain = &metalLayerDesc;
    #elif defined(__linux__) // TODO: Switch to Wayland
        wgpu::SurfaceSourceXlibWindow x11Desc = {};
        x11Desc.display = glfwGetX11Display();
        x11Desc.window = glfwGetX11Window(window);
        surfaceDesc.nextInChain = &x11Desc;
    #endif

    renderer.surface = renderer.instance.CreateSurface(&surfaceDesc);
    if (!renderer.surface) {
        return std::unexpected(RendererCreateError::FailedToCreateSurface);
    }

    wgpu::RequestAdapterOptions adapterOpts = {};
    adapterOpts.compatibleSurface = renderer.surface;

    bool adapterResult = false;
    auto adapterFuture = renderer.instance.RequestAdapter(
        &adapterOpts,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
            if (status == wgpu::RequestAdapterStatus::Success) {
                adapterResult = true;
                renderer.adapter = std::move(adapter);
            }
        }
    );
    renderer.instance.WaitAny(adapterFuture, UINT64_MAX);
    if (!adapterResult) {
        return std::unexpected(RendererCreateError::FailedToCreateAdapter);
    }

    bool deviceResult = false;
    auto deviceFuture = renderer.adapter.RequestDevice(
        nullptr,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestDeviceStatus status, wgpu::Device device, wgpu::StringView message) {
            if (status == wgpu::RequestDeviceStatus::Success) {
                deviceResult = true;
                renderer.device = std::move(device);
            }
        }
    );
    renderer.instance.WaitAny(deviceFuture, UINT64_MAX);
    if (!deviceResult){
        return std::unexpected(RendererCreateError::FailedToCreateDevice);
    }

    wgpu::SurfaceCapabilities capabilities;
    renderer.surface.GetCapabilities(renderer.adapter, &capabilities);
    renderer.format = capabilities.formats[0];

    wgpu::SurfaceConfiguration config{
        .device = renderer.device,
        .format = renderer.format,
        .width = width,
        .height = height,
        .presentMode = wgpu::PresentMode::Fifo,
    };
    renderer.surface.Configure(&config);

    wgpu::ShaderSourceWGSL wgsl;
    wgsl.code = shaders::triangle_wgsl;

    wgpu::ShaderModuleDescriptor shaderDesc{};
    shaderDesc.nextInChain = &wgsl;
    wgpu::ShaderModule shaderModule = renderer.device.CreateShaderModule(&shaderDesc);

    wgpu::ColorTargetState colorTarget{
        .format = renderer.format,
        .writeMask = wgpu::ColorWriteMask::All,
    };

    wgpu::FragmentState fragment{
        .module = shaderModule,
        .entryPoint = "fs_main",
        .targetCount = 1,
        .targets = &colorTarget,
    };

    wgpu::RenderPipelineDescriptor pipelineDesc{
        .vertex = {
            .module = shaderModule,
            .entryPoint = "vs_main",
        },
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleStrip,
            .frontFace = wgpu::FrontFace::CCW,
            .cullMode = wgpu::CullMode::None,
        },
        .fragment = &fragment,
    };

    renderer.pipeline = renderer.device.CreateRenderPipeline(&pipelineDesc);
    renderer.queue = renderer.device.GetQueue();

    return {};
}

void Renderer::render(Renderer& renderer) {
    wgpu::SurfaceTexture surfaceTexture;
    renderer.surface.GetCurrentTexture(&surfaceTexture);

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
        return;
    }

    wgpu::TextureView view = surfaceTexture.texture.CreateView();

    wgpu::RenderPassColorAttachment colorAttachment{
        .view = view,
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {1.0, 1.0, 1.0, 0.0},
    };

    wgpu::RenderPassDescriptor renderPassDesc{
        .colorAttachmentCount = 1,
        .colorAttachments = &colorAttachment,
    };

    wgpu::CommandEncoder encoder = renderer.device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);
    pass.SetPipeline(renderer.pipeline);
    pass.Draw(4, 1, 0, 0);
    pass.End();

    wgpu::CommandBuffer commands = encoder.Finish();
    renderer.queue.Submit(1, &commands);

    renderer.surface.Present();
    renderer.instance.ProcessEvents();
}


void Renderer::destroy(Renderer& renderer) {
    renderer.surface.Unconfigure();
    renderer.surface = nullptr;

    renderer.pipeline = nullptr;
    renderer.queue = nullptr;
    renderer.device = nullptr;
    renderer.adapter = nullptr;
    renderer.instance = nullptr;
}
