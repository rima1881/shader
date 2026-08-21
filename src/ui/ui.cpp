#include "ui/components/square.h"
#include "ui/rendered.h"
#include <ui/ui.h>

void UI::create(UI& ui) {
    add_square(ui, Square{
        .x =  -0.5f,
        .y= -0.5f,
        .width = 1.0f,
        .height = 1.0f,
        .color = {255, 0, 0, 255},
    });
    add_square(ui, Square{
        .x = -0.9f,
        .y = 0.3f,
        .width = 0.3f,
        .height = 0.3f,
        .color = {0, 255, 0, 255},
    });
}

void UI::add_square(UI& ui, const Square& square) {
    ui.components.push_back(square);
}

std::vector<uint32_t> UI::get_all_indices(const UI& ui) {
    uint32_t offset = 0;
    std::vector<uint32_t> data;
    for (const auto& comp : ui.components) {
        auto indices = rendered::get_indices(comp);
        for (auto& index : indices) {
            index += offset;
        }
        data.insert(data.end(), indices.begin(), indices.end());
        offset += rendered::get_vertex_count(comp);

        // end of component
        data.push_back(0xFFFFFFFFU);
    }

    return data;
}

std::vector<float> UI::get_all_vertices(const UI& ui) {
    std::vector<float> data;
    for (const auto& comp : ui.components) {
        auto vertices = rendered::get_vertices(comp);
        data.insert(data.end(), vertices.begin(), vertices.end());
    }
    return data;
}

std::vector<float> UI::get_all_colors(const UI& ui) {
    std::vector<float> data = {1, 0, 0, 1};
    // TODO: temporary commented out
    // for (const auto& comp : ui.components) {
    //     auto [r, g, b, a] = rendered::get_color(comp);
    //     for (float c : {r, g, b, a}) {
    //         data.push_back(c / 255.0f);
    //     }
    // }
    return data;
}
