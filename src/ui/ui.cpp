#include <ui/ui.h>

void UI::create(UI& ui) {
    add_square(ui, Square{ .x =  -0.5f, .y= -0.5f, .width = 1.0f, .height = 1.0f });
}


void UI::add_square(UI& ui, const Square& square) {
    ui.components.push_back(square);
}

std::vector<float> UI::get_all_vertices(const UI& ui) {
    std::vector<float> data;
    for (const auto& comp : ui.components) {
        auto vertices = rendered::get_vertices(comp);
        data.insert(data.end(), vertices.begin(), vertices.end());
    }
    return data;
}

uint32_t UI::get_all_vertex_count(const UI& ui) {
    uint32_t count = 0;
    for (const auto& comp : ui.components) {
        count += rendered::get_vertex_count(comp);
    }
    return count;
}
