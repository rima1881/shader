
#ifndef GHOAST_H
#define GHOAST_H

#include <backend/entity/entity_traits.h>
#include <backend/map/position.h>

struct Ghost{
    Position position;

    static bool move(Ghost& ghoast, Position pos);
    static Position get_position(const Ghost& ghost);
};

static_assert(EntityConcept<Ghost>);

#endif
