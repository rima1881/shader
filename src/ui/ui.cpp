#include <ui/ui.h>

void UI::create(UI& ui) {
    add_square(ui, Square{ .x =  -0.5f, .y= -0.5f, .width = 1.0f, .height = 1.0f });
    add_square(ui, Square{ .x = -0.9f, .y = 0.3f, .width = 0.3f, .height = 0.3f });
}

void UI::add_square(UI& ui, const Square& square) {
    ui.components.push_back(square);
}

uint32_t UI::get_all_vertex_count(const UI& ui) {
    uint32_t count = 0;
    for (const auto& comp : ui.components) {
        count += rendered::get_vertex_count(comp);
    }
    return count;
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

std::vector<float> UI::get_all_colors(const UI& ui) {
    std::vector<float> data;
    for (const auto& comp : ui.components) {
        auto colors = rendered::get_colors(comp);
        data.insert(data.end(), colors.begin(), colors.end()); // TODO: probably wrong
    }
    return data;
}
