
#ifndef PACMAN_H
#define PACMAN_H

#include <backend/entity/entity_traits.h>
#include <backend/map/position.h>

struct Pacman {
    Position pos;

    static bool move(Pacman& pacman, Position pos);
    static Position get_position(const Pacman& pacman);
};


static_assert(EntityConcept<Pacman>);

#endif
