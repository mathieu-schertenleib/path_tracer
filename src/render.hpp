#ifndef RENDER_HPP
#define RENDER_HPP

#include "trace.hpp"
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

struct Material
{
    float3 albedo;
    float3 emissivity;
};

struct Scene
{
    Camera camera;
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    float3 background_color;
};

enum struct Sample_type
{
    color,
    albedo,
    normal,
    barycentric,
    primitive_id,
    material_id,
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
                                  u32 &rng_state,
                                  u32 color_rng_state);

#endif // RENDER_HPP
