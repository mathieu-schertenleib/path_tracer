#ifndef RENDER_HPP
#define RENDER_HPP

#include "geometry.hpp"
#include "vec3.hpp"

struct Camera
{
    float3 position;
    float3 direction;
    float3 local_x;
    float3 local_y;
    float focal_length;
    float sensor_width;
    float sensor_height;
};

struct Scene
{
    Camera camera;
    std::vector<Triangle> triangles;
    float3 background_color;
};

enum struct Sample_type
{
    radiance,
    base_color,
    primitive_id,
    material_id,
    uv
};

[[nodiscard]] Camera create_camera(float3 position,
                                   float3 direction,
                                   float3 up,
                                   float focal_length,
                                   float sensor_width,
                                   float sensor_height);

[[nodiscard]] Scene cornell_box();

[[nodiscard]] float3 sample_pixel(const Scene &scene,
                                  int pixel_i,
                                  int pixel_j,
                                  int image_width,
                                  int image_height,
                                  Sample_type sample_type,
                                  u32 &rng_state);

#endif // RENDER_HPP
