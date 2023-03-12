#include "trace.hpp"

namespace
{

constexpr void intersect(const Ray &ray,
                         const Triangle &triangle,
                         u32 triangle_id,
                         f32 t_min,
                         f32 &t_max,
                         Ray_payload &payload)
{
    // Möller-Trumbore

    constexpr f32 epsilon {1e-8f};
    const auto edge1 = triangle.vertex1 - triangle.vertex0;
    const auto edge2 = triangle.vertex2 - triangle.vertex0;
    const auto h = vec::cross(ray.direction, edge2);
    const auto a = vec::dot(edge1, h);
    if (a > -epsilon && a < epsilon) [[unlikely]]
    {
        // The ray is parallel to the triangle
        return;
    }
    const auto f = 1.0f / a;
    const auto s = ray.origin - triangle.vertex0;
    const auto u = f * vec::dot(s, h);
    const auto q = vec::cross(s, edge1);
    const auto v = f * vec::dot(ray.direction, q);
    const auto t = f * vec::dot(edge2, q);
    const auto hit_condition = (u >= 0.0f) & (u <= 1.0f) & (v >= 0.0f) &
                               (u + v <= 1.0f) & (t > t_min) & (t < t_max);
    if (hit_condition) [[unlikely]]
    {
        t_max = t;
        payload.position = ray.origin + t * ray.direction;
        payload.u = u;
        payload.v = v;
        payload.primitive_id = triangle_id;
    }
}

} // namespace

Ray_payload intersect(const Ray &ray, const std::vector<Triangle> &triangles)
{
    constexpr f32 t_min {1e-6f};
    constexpr f32 t_max {std::numeric_limits<f32>::max()};
    f32 t {t_max};
    Ray_payload payload {};
    payload.primitive_id = 0xffffffffu;
    for (u32 i {}; i < triangles.size(); ++i)
    {
        intersect(ray, triangles[i], i, t_min, t, payload);
    }
    return payload;
}
