#include <ui/components/square.h>

std::vector<float> Square::get_vertices(const Square& square) {
    float x0 = square.x;
    float y0 = square.y;
    float x1 = square.x + square.width;
    float y1 = square.y + square.height;

    return {
        x0, y0,
        x1, y0,
        x0, y1,
        x1, y1,
    };
}

uint32_t Square::get_vertex_count(const Square& square) {
    return 4;
}
