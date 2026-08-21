
#ifndef ENTITY_H
#define ENTITY_H

#include <backend/map/position.h>
#include <variant>
#include <backend/entity/ghost.h>
#include <backend/entity/pacman.h>

namespace Entity{
    using Item = std::variant<Pacman, Ghost>;

    Position get_position(const Item& item);
}

#endif
