
#ifndef TILE_ITEM_H
#define TILE_ITEM_H

#include <variant>

namespace TileItem{

    struct Wall {};
    struct Point{};
    struct Empty{};

    using Item = std::variant<Wall, Point, Empty>;
}


#endif
