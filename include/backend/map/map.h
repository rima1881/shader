
#ifndef MAP_H
#define MAP_H

#include <backend/map/tile.h>
#include <array>

struct Map{
    std::array<Tile, 64 * 64> tiles;
};

#endif
