
#ifndef ENTITY_TRAITS_H
#define ENTITY_TRAITS_H

#include <concepts>
#include <backend/map/position.h>

template<typename T>
concept EntityConcept = requires (T& t, const Position& pos) {
    { T::move(t, pos) } -> std::same_as<bool>;
    { T::get_position(t)} -> std::same_as<Position>;
};

#endif
