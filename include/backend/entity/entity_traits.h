
#ifndef ENTITY_TRAITS_H
#define ENTITY_TRAITS_H

#include <concepts>
#include <backend/map/position.h>

template<typename T>
concept MovingConcept = requires (T& t, const Position& pos) {
    { T::move(t, pos) } -> std::same_as<bool>;
};

template<typename T>
concept InputTakingConcept = requires (T& t) {
    { T::take_input(t)} -> std::same_as<bool>;
};

#endif
