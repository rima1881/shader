#include <webgpu/webgpu.h>
#include <app.h>

int main() {
    App app;
    auto result = App::create(app, 800, 600, "Bita");
    if (!result) {
        return 1;
    }

    App::run(app);
    App::destroy(app);


    return 0;
}
