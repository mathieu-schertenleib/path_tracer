#ifndef TRACE_HPP
#define TRACE_HPP

#include "definitions.hpp"
#include "vec.hpp"

#include <vector>

struct Ray
{
    f32v3 origin;
    f32v3 direction;
};

struct Triangle
{
    f32v3 vertex0;
    f32v3 vertex1;
    f32v3 vertex2;
    u32 material_id;
};

struct Ray_payload
{
    f32v3 position;
    f32 u;
    f32 v;
    u32 primitive_id;
};

[[nodiscard]] Ray_payload intersect(const Ray &ray,
                                    const std::vector<Triangle> &triangles);

#endif // TRACE_HPP
