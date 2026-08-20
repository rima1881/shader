#include <ui/rendered.h>

std::vector<float> rendered::get_vertices(const component& comp) {
    return std::visit([](const auto& arg) {
           return std::decay_t<decltype(arg)>::get_vertices(arg);
    }, comp);
}

uint32_t rendered::get_vertex_count(const component& comp) {
    return std::visit([](const auto& arg) {
           return std::decay_t<decltype(arg)>::get_vertex_count(arg);
    }, comp);
}

std::vector<uint32_t> rendered::get_indices(const component& comp) {
    return std::visit([](const auto& arg) {
           return std::decay_t<decltype(arg)>::get_indices(arg);
    }, comp);
}

std::array<uint8_t, 4> rendered::get_color(const component& comp) {
    return std::visit([](const auto& arg) {
           return std::decay_t<decltype(arg)>::get_color(arg);
    }, comp);
}
