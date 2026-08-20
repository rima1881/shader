
#ifndef UI_UI_H
#define UI_UI_H

#include <vector>
#include <cstdint>
#include <ui/rendered.h>

struct UI {
    std::vector<rendered::component> components;

    static void create(UI& ui);
    static void add_square(UI& ui, const Square& square);
    static std::vector<float> get_all_vertices(const UI& ui);
    static std::vector<uint32_t> get_all_indices(const UI& ui);
    static std::vector<float> get_all_colors(const UI& ui);
};

#endif // UI_UI_H
