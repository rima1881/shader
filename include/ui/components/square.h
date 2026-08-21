
#ifndef UI_COMPONENTS_SQUARE_H
#define UI_COMPONENTS_SQUARE_H

#include <array>
#include <cstdint>
#include <ui/ui_traits.h>

struct Square {
    float x, y, width, height;
    std::array<uint8_t, 4> color;

    static std::vector<float> get_vertices(const Square& square);
    static uint32_t get_vertex_count(const Square& square);
    static std::vector<uint32_t> get_indices(const Square& square);
    static std::array<uint8_t, 4> get_color(const Square& square);
};

static_assert(RenderedConcept<Square>);

#endif // UI_COMPONENTS_SQUARE_H
