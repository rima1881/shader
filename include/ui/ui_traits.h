
#ifndef I_UI_TRAITS_H
#define I_UI_TRAITS_H

#include <concepts>
#include <vector>
#include <cstdint>

template <typename T>
concept RenderedConcept = requires(const T& t) {
    { T::get_vertices(t) } -> std::same_as<std::vector<float>>;
    { T::get_vertex_count(t) } -> std::same_as<uint32_t>;
};

#endif // I_UI_TRAITS_H
