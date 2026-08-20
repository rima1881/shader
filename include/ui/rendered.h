
#ifndef UI_RENDERED_H
#define UI_RENDERED_H

#include <cstdint>
#include <variant>
#include <vector>
#include <ui/components/square.h>

namespace rendered {
    using component = std::variant<Square>;

    std::vector<float> get_vertices(const component& comp);
    uint32_t get_vertex_count(const component& comp);
    std::vector<uint32_t> get_indices(const component& comp);
    std::array<uint8_t, 4> get_color(const component& comp);
}

#endif // UI_RENDERED_H
