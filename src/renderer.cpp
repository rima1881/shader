
#include <renderer.h>
#include <webgpu/webgpu_cpp.h>

std::expected<void, RendererCreateError> Renderer::create(Renderer& renderer, GLFWwindow* window, const UI& ui, uint32_t width, uint32_t height) {
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

    wgpu::VertexAttribute positionAttrib{
        .format = wgpu::VertexFormat::Float32x2,
        .offset = 0,
        .shaderLocation = 0,
    };

    wgpu::VertexBufferLayout vertexLayout{
        .stepMode = wgpu::VertexStepMode::Vertex,
        .arrayStride = 2 * sizeof(float),   // 8 bytes between vertices
        .attributeCount = 1,
        .attributes = &positionAttrib,
    };

    wgpu::RenderPipelineDescriptor pipelineDesc{
        .vertex = {
            .module = shaderModule,
            .entryPoint = "vs_main",
            .bufferCount = 1,
            .buffers = &vertexLayout,
        },
        .primitive = {
            .topology = wgpu::PrimitiveTopology::TriangleStrip,
            .stripIndexFormat = wgpu::IndexFormat::Uint32,
            .frontFace = wgpu::FrontFace::CCW,
            .cullMode = wgpu::CullMode::None,
        },
        .fragment = &fragment,
    };

    renderer.pipeline = renderer.device.CreateRenderPipeline(&pipelineDesc);
    renderer.queue = renderer.device.GetQueue();

    auto vertexData = UI::get_all_vertices(ui);
    const size_t vertexDataSize = vertexData.size() * sizeof(float);

    wgpu::BufferDescriptor vertexBufferDesc{
        .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
        .size = vertexDataSize
    };

    renderer.vertexBuffer = renderer.device.CreateBuffer(&vertexBufferDesc);
    renderer.queue.WriteBuffer(renderer.vertexBuffer, 0, vertexData.data(), vertexDataSize);


    auto indexData = UI::get_all_indices(ui);
    const size_t indexDataSize = indexData.size() * sizeof(uint32_t);

    wgpu::BufferDescriptor indexBufferDesc{
        .usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst,
        .size = indexDataSize,
    };
    renderer.indexBuffer = renderer.device.CreateBuffer(&indexBufferDesc);
    renderer.queue.WriteBuffer(renderer.indexBuffer, 0, indexData.data(), indexDataSize);

    auto colorData = UI::get_all_colors(ui);
    const size_t colorDataSize = colorData.size() * sizeof(float);

    wgpu::BufferDescriptor colorBufferDesc{
        .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
        .size = colorDataSize,
    };
    renderer.colorBuffer = renderer.device.CreateBuffer(&colorBufferDesc);

    return {};
}

void Renderer::render(Renderer& renderer, const UI& ui) {
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

    auto vertexData = UI::get_all_vertices(ui);
    auto indexData = UI::get_all_indices(ui);

    // buffer is too small to hold all vertices
    if (vertexData.size() * sizeof(float) > renderer.vertexBuffer.GetSize()) {
        wgpu::BufferDescriptor vertexBufferDesc{
            .usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst,
            .size = vertexData.size() * sizeof(float),
        };
        renderer.vertexBuffer = renderer.device.CreateBuffer(&vertexBufferDesc);
    }

    if (indexData.size() * sizeof(uint32_t) > renderer.indexBuffer.GetSize()) {
        wgpu::BufferDescriptor indexBufferDesc{
            .usage = wgpu::BufferUsage::Index | wgpu::BufferUsage::CopyDst,
            .size = indexData.size() * sizeof(uint32_t),
        };
        renderer.indexBuffer = renderer.device.CreateBuffer(&indexBufferDesc);
    }

    if (colorDataSize > renderer.colorBuffer.GetSize()) {
        wgpu::BufferDescriptor colorBufferDesc{
            .usage = wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst,
            .size = colorDataSize,
        };
        renderer.colorBuffer = renderer.device.CreateBuffer(&colorBufferDesc);
    }

    renderer.queue.WriteBuffer(renderer.vertexBuffer, 0, vertexData.data(), vertexData.size() * sizeof(float));
    renderer.queue.WriteBuffer(renderer.indexBuffer, 0, indexData.data(), indexData.size() * sizeof(uint32_t));

    pass.SetPipeline(renderer.pipeline);
    pass.SetVertexBuffer(0, renderer.vertexBuffer);
    pass.SetIndexBuffer(renderer.indexBuffer, wgpu::IndexFormat::Uint32);
    pass.DrawIndexed(static_cast<uint32_t>(indexData.size()), 1, 0, 0, 0);
    pass.End();

    wgpu::CommandBuffer commands = encoder.Finish();
    renderer.queue.Submit(1, &commands);

    renderer.surface.Present();
    renderer.instance.ProcessEvents();
}


void Renderer::destroy(Renderer& renderer) {
    renderer.surface.Unconfigure();
    renderer.surface = nullptr;

    renderer.colorBuffer = nullptr;
    renderer.indexBuffer = nullptr;
    renderer.vertexBuffer = nullptr;
    renderer.pipeline = nullptr;
    renderer.queue = nullptr;
    renderer.device = nullptr;
    renderer.adapter = nullptr;
    renderer.instance = nullptr;
}
