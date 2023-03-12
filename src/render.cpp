#include "render.hpp"

#include "random.hpp"

namespace
{

[[nodiscard]] constexpr f32v3 random_color(u32 base_state, u32 id) noexcept
{
    auto rng_state = seed(base_state + id);
    return {random(rng_state), random(rng_state), random(rng_state)};
}

[[nodiscard]] f32v3 radiance(const Scene &scene, const Ray &ray, u32 &rng_state)
{
    f32v3 accumulated_color {};
    f32v3 accumulated_reflectance {1.0f, 1.0f, 1.0f};
    auto r = ray;
    for (int depth {};; ++depth)
    {
        const auto payload = intersect(r, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return scene.background_color;
        }

        const auto &triangle = scene.triangles[payload.primitive_id];
        const auto triangle_normal =
            vec::normalize(vec::cross(triangle.vertex1 - triangle.vertex0,
                                      triangle.vertex2 - triangle.vertex0));
        const auto normal = vec::dot(triangle_normal, r.direction) < 0.0f
                                ? triangle_normal
                                : -triangle_normal;
        auto albedo = scene.materials[triangle.material_id].albedo;
        const auto p = albedo.x > albedo.y && albedo.x > albedo.z ? albedo.x
                       : albedo.y > albedo.z                      ? albedo.y
                                                                  : albedo.z;
        accumulated_color += accumulated_reflectance *
                             scene.materials[triangle.material_id].emissivity;
        if (depth > 5)
        {
            if (random(rng_state) >= p || p < 1e-6f)
            {
                return accumulated_color;
            }
            else
            {
                albedo *= (1.0f / p);
            }
        }
        accumulated_reflectance *= albedo;

        auto new_direction = normal + random_unit_vector(rng_state);
        if (vec::length(new_direction) < 1e-6f)
        {
            new_direction = normal;
        }
        else
        {
            new_direction = vec::normalize(new_direction);
        }
        r.origin = payload.position + 1e-6f * normal;
        r.direction = new_direction;
    }
}

} // namespace

Camera create_camera(f32v3 position,
                     f32v3 direction,
                     f32v3 up,
                     f32 focal_length,
                     f32 sensor_width,
                     f32 sensor_height)
{
    const auto local_x = vec::normalize(vec::cross(direction, up));
    const auto local_y = vec::cross(local_x, direction);
    return Camera {.position = position,
                   .direction = direction,
                   .local_x = local_x,
                   .local_y = local_y,
                   .focal_length = focal_length,
                   .sensor_width = sensor_width,
                   .sensor_height = sensor_height};
}

Scene cornell_box()
{
    // Adapted from http://www.graphics.cornell.edu/online/box/data.html

    // x towards left
    // y towards ceiling
    // z towards back
    // (0, 0, 0) is the front bottom right corner

    constexpr f32v3 floor[] {{552.8f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 559.2f},
                             {549.6f, 0.0f, 559.2f}};

    constexpr f32v3 light[] {{343.0f, 520.0f, 227.0f},
                             {343.0f, 520.0f, 332.0f},
                             {213.0f, 520.0f, 332.0f},
                             {213.0f, 520.0f, 227.0f}};

    constexpr f32v3 ceiling[] {{556.0f, 548.8f, 0.0f},
                               {556.0f, 548.8f, 559.2f},
                               {0.0f, 548.8f, 559.2f},
                               {0.0f, 548.8f, 0.0f}};

    constexpr f32v3 back_wall[] {{549.6f, 0.0f, 559.2f},
                                 {0.0f, 0.0f, 559.2f},
                                 {0.0f, 548.8f, 559.2f},
                                 {556.0f, 548.8f, 559.2f}};

    constexpr f32v3 green_wall[] {{0.0f, 0.0f, 559.2f},
                                  {0.0f, 0.0f, 0.0f},
                                  {0.0f, 548.8f, 0.0f},
                                  {0.0f, 548.8f, 559.2f}};

    constexpr f32v3 red_wall[] {{552.8f, 0.0f, 0.0f},
                                {549.6f, 0.0f, 559.2f},
                                {556.0f, 548.8f, 559.2f},
                                {556.0f, 548.8f, 0.0f}};

    constexpr f32v3 short_block[] {
        {130.0f, 165.0f, 65.0f},  {82.0f, 165.0f, 225.0f},
        {240.0f, 165.0f, 272.0f}, {290.0f, 165.0f, 114.0f},
        {290.0f, 0.0f, 114.0f},   {290.0f, 165.0f, 114.0f},
        {240.0f, 165.0f, 272.0f}, {240.0f, 0.0f, 272.0f},
        {130.0f, 0.0f, 65.0f},    {130.0f, 165.0f, 65.0f},
        {290.0f, 165.0f, 114.0f}, {290.0f, 0.0f, 114.0f},
        {82.0f, 0.0f, 225.0f},    {82.0f, 165.0f, 225.0f},
        {130.0f, 165.0f, 65.0f},  {130.0f, 0.0f, 65.0f},
        {240.0f, 0.0f, 272.0f},   {240.0f, 165.0f, 272.0f},
        {82.0f, 165.0f, 225.0f},  {82.0f, 0.0f, 225.0f}};

    constexpr f32v3 tall_block[] {
        {423.0f, 330.0f, 247.0f}, {265.0f, 330.0f, 296.0f},
        {314.0f, 330.0f, 456.0f}, {472.0f, 330.0f, 406.0f},
        {423.0f, 0.0f, 247.0f},   {423.0f, 330.0f, 247.0f},
        {472.0f, 330.0f, 406.0f}, {472.0f, 0.0f, 406.0f},
        {472.0f, 0.0f, 406.0f},   {472.0f, 330.0f, 406.0f},
        {314.0f, 330.0f, 456.0f}, {314.0f, 0.0f, 456.0f},
        {314.0f, 0.0f, 456.0f},   {314.0f, 330.0f, 456.0f},
        {265.0f, 330.0f, 296.0f}, {265.0f, 0.0f, 296.0f},
        {265.0f, 0.0f, 296.0f},   {265.0f, 330.0f, 296.0f},
        {423.0f, 330.0f, 247.0f}, {423.0f, 0.0f, 247.0f}};

    constexpr Material white {.albedo = {0.75f, 0.75f, 0.75f},
                              .emissivity = {}};

    constexpr Material green {.albedo = {0.25f, 0.75f, 0.25f},
                              .emissivity = {}};

    constexpr Material red {.albedo = {0.75f, 0.25f, 0.25f}, .emissivity = {}};

    constexpr Material emissive {.albedo = {},
                                 .emissivity = {12.0f, 12.0f, 12.0f}};

    return Scene {
        .camera = create_camera(
            {278, 273, -800}, {0, 0, 1}, {0, 1, 0}, 0.035f, 0.025f, 0.025f),
        .triangles =
            {{floor[0], floor[1], floor[2], 0},
             {floor[0], floor[2], floor[3], 0},
             {light[0], light[1], light[2], 3},
             {light[0], light[2], light[3], 3},
             {ceiling[0], ceiling[1], ceiling[2], 0},
             {ceiling[0], ceiling[2], ceiling[3], 0},
             {back_wall[0], back_wall[1], back_wall[2], 0},
             {back_wall[0], back_wall[2], back_wall[3], 0},
             {green_wall[0], green_wall[1], green_wall[2], 1},
             {green_wall[0], green_wall[2], green_wall[3], 1},
             {red_wall[0], red_wall[1], red_wall[2], 2},
             {red_wall[0], red_wall[2], red_wall[3], 2},
             {short_block[0 + 0], short_block[0 + 1], short_block[0 + 2], 0},
             {short_block[0 + 0], short_block[0 + 2], short_block[0 + 3], 0},
             {short_block[4 + 0], short_block[4 + 1], short_block[4 + 2], 0},
             {short_block[4 + 0], short_block[4 + 2], short_block[4 + 3], 0},
             {short_block[8 + 0], short_block[8 + 1], short_block[8 + 2], 0},
             {short_block[8 + 0], short_block[8 + 2], short_block[8 + 3], 0},
             {short_block[12 + 0], short_block[12 + 1], short_block[12 + 2], 0},
             {short_block[12 + 0], short_block[12 + 2], short_block[12 + 3], 0},
             {short_block[16 + 0], short_block[16 + 1], short_block[16 + 2], 0},
             {short_block[16 + 0], short_block[16 + 2], short_block[16 + 3], 0},
             {tall_block[0 + 0], tall_block[0 + 1], tall_block[0 + 2], 0},
             {tall_block[0 + 0], tall_block[0 + 2], tall_block[0 + 3], 0},
             {tall_block[4 + 0], tall_block[4 + 1], tall_block[4 + 2], 0},
             {tall_block[4 + 0], tall_block[4 + 2], tall_block[4 + 3], 0},
             {tall_block[8 + 0], tall_block[8 + 1], tall_block[8 + 2], 0},
             {tall_block[8 + 0], tall_block[8 + 2], tall_block[8 + 3], 0},
             {tall_block[12 + 0], tall_block[12 + 1], tall_block[12 + 2], 0},
             {tall_block[12 + 0], tall_block[12 + 2], tall_block[12 + 3], 0},
             {tall_block[16 + 0], tall_block[16 + 1], tall_block[16 + 2], 0},
             {tall_block[16 + 0], tall_block[16 + 2], tall_block[16 + 3], 0}},
        .materials = {white, green, red, emissive},
        .background_color = {}};
}

f32v3 sample_pixel(const Scene &scene,
                   int pixel_i,
                   int pixel_j,
                   int image_width,
                   int image_height,
                   Sample_type sample_type,
                   u32 &rng_state,
                   u32 color_rng_state)
{
    const auto x = (static_cast<f32>(pixel_j) + random(rng_state)) /
                       static_cast<f32>(image_width) -
                   0.5f;
    const auto y =
        (static_cast<f32>(image_height - 1 - pixel_i) + random(rng_state)) /
            static_cast<f32>(image_height) -
        0.5f;
    const Ray ray {.origin = scene.camera.position,
                   .direction = vec::normalize(
                       scene.camera.focal_length * scene.camera.direction +
                       x * scene.camera.sensor_width * scene.camera.local_x +
                       y * scene.camera.sensor_height * scene.camera.local_y)};

    switch (sample_type)
    {
    case Sample_type::color:
    {
        return radiance(scene, ray, rng_state);
    }
    case Sample_type::albedo:
    {
        const auto payload = intersect(ray, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return scene.background_color;
        }
        return scene
            .materials[scene.triangles[payload.primitive_id].material_id]
            .albedo;
    }
    case Sample_type::normal:
    {
        const auto payload = intersect(ray, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return {};
        }
        const auto &triangle = scene.triangles[payload.primitive_id];
        const auto normal =
            vec::normalize(vec::cross(triangle.vertex1 - triangle.vertex0,
                                      triangle.vertex2 - triangle.vertex0));
        return (normal + f32v3 {1.0f, 1.0f, 1.0f}) * 0.5f;
    }
    case Sample_type::barycentric:
    {
        const auto payload = intersect(ray, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return {};
        }
        return {1.0f - payload.u - payload.v, payload.u, payload.v};
    }
    case Sample_type::primitive_id:
    {
        const auto payload = intersect(ray, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return {};
        }
        return random_color(color_rng_state, payload.primitive_id);
    }
    case Sample_type::material_id:
    {
        const auto payload = intersect(ray, scene.triangles);
        if (payload.primitive_id == 0xffffffffu)
        {
            return {};
        }
        return random_color(color_rng_state,
                            scene.triangles[payload.primitive_id].material_id);
    }
    }

    return {};
}
