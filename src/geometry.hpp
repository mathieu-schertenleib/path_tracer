#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "definitions.hpp"
#include "vec3.hpp"

#include <vector>

struct Ray
{
    float3 origin;
    float3 direction;
};

struct Triangle
{
    float3 vertex0;
    float3 vertex1;
    float3 vertex2;
};

struct Ray_payload
{
    float3 position;
    float u;
    float v;
    u32 primitive_id;
};

[[nodiscard]] Ray_payload intersect(const Ray &ray,
                                    const std::vector<Triangle> &triangles);

#endif // GEOMETRY_HPP
