#ifndef RENDER_SURFACE_H
#define RENDER_SURFACE_H

#include <cstddef>
struct Position {
    float x, y;
};

struct RenderSurface {
    size_t num_vertices;
    Position vertices[];
};

const RenderSurface render_surface = {
    .num_vertices = 6,
    .vertices = {
        // First Triangle
        Position{
            -1.0,
            -1.0
        },
        Position{
            -1.0,
            1.0
        },
        Position{
            1.0,
            1.0
        },
        // Second triangle
        Position{
            -1.0,
            -1.0
        },
        Position{
            1.0,
            -1.0
        },
        Position{
            1.0,
            1.0
        }
    }
};

#endif
