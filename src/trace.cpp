#include "trace.hpp"

namespace
{

constexpr void intersect(const Ray &ray,
                         const Triangle &triangle,
                         u32 triangle_id,
                         float t_min,
                         float &t_max,
                         Ray_payload &payload)
{
    // Möller-Trumbore

    constexpr float epsilon {1e-8f};
    const auto edge1 = triangle.vertex1 - triangle.vertex0;
    const auto edge2 = triangle.vertex2 - triangle.vertex0;
    const auto h = cross(ray.direction, edge2);
    const auto a = dot(edge1, h);
    if (a > -epsilon && a < epsilon)
    {
        // The Ray is parallel to the triangle
        return;
    }
    const auto f = 1.0f / a;
    const auto s = ray.origin - triangle.vertex0;
    const auto u = f * dot(s, h);
    const auto q = cross(s, edge1);
    const auto v = f * dot(ray.direction, q);
    if (u < 0.0f || u > 1.0f || v < 0.0f || u + v > 1.0f)
    {
        return;
    }
    const auto t = f * dot(edge2, q);
    if (t > t_min && t < t_max)
    {
        t_max = t;
        payload = Ray_payload {.position = ray.origin + t * ray.direction,
                               .u = u,
                               .v = v,
                               .primitive_id = triangle_id};
    }
}

} // namespace

Ray_payload intersect(const Ray &ray, const std::vector<Triangle> &triangles)
{
    constexpr float t_min {1e-6f};
    constexpr float t_max {1e6f};
    float t {t_max};
    Ray_payload payload {};
    payload.primitive_id = 0xffffffffu;
    for (u32 i {}; i < triangles.size(); ++i)
    {
        intersect(ray, triangles[i], i, t_min, t, payload);
    }
    return payload;
}
