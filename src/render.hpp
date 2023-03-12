#ifndef RENDER_HPP
#define RENDER_HPP

#include "trace.hpp"
#include "vec.hpp"

struct Camera
{
    f32v3 position;
    f32v3 direction;
    f32v3 local_x;
    f32v3 local_y;
    f32 focal_length;
    f32 sensor_width;
    f32 sensor_height;
};

struct Material
{
    f32v3 albedo;
    f32v3 emissivity;
};

struct Scene
{
    Camera camera;
    std::vector<Triangle> triangles;
    std::vector<Material> materials;
    f32v3 background_color;
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

[[nodiscard]] Camera create_camera(f32v3 position,
                                   f32v3 direction,
                                   f32v3 up,
                                   f32 focal_length,
                                   f32 sensor_width,
                                   f32 sensor_height);

[[nodiscard]] Scene cornell_box();

[[nodiscard]] f32v3 sample_pixel(const Scene &scene,
                                 int pixel_i,
                                 int pixel_j,
                                 int image_width,
                                 int image_height,
                                 Sample_type sample_type,
                                 u32 &rng_state,
                                 u32 color_rng_state);

#endif // RENDER_HPP
