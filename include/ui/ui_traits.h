
#ifndef I_UI_TRAITS_H
#define I_UI_TRAITS_H

#include <concepts>
#include <vector>
#include <cstdint>
#include <array>

template <typename T>
concept RenderedConcept = requires(const T& t) {
    { T::get_vertices(t) } -> std::same_as<std::vector<float>>;
    { T::get_vertex_count(t) } -> std::same_as<uint32_t>;
    { T::get_indices(t) } -> std::same_as<std::vector<uint32_t>>;
    { T::color(t) } -> std::same_as<std::array<uint8_t, 4>>;
};

#endif // I_UI_TRAITS_H
