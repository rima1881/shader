
#ifndef GHOAST_H
#define GHOAST_H

#include <backend/entity/entity_traits.h>
#include <backend/map/position.h>

struct Ghoast{
    Position position;

    static bool move(Ghoast& ghoast, Position pos);
};

static_assert(MovingConcept<Ghoast>);

#endif
