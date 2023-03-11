#include "render.hpp"

#include "random.hpp"

namespace
{

[[nodiscard]] constexpr float3 random_color(u32 id) noexcept
{
    auto rng_state = seed(id + 1);
    return {random(rng_state), random(rng_state), random(rng_state)};
}

} // namespace

Camera create_camera(float3 position,
                     float3 direction,
                     float3 up,
                     float focal_length,
                     float sensor_width,
                     float sensor_height)
{
    const auto local_x = normalize(cross(direction, up));
    const auto local_y = cross(local_x, direction);
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

    constexpr float3 floor[] {{552.8f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 559.2f},
                              {549.6f, 0.0f, 559.2f}};

    constexpr float3 light[] {{343.0f, 520.0f, 227.0f},
                              {343.0f, 520.0f, 332.0f},
                              {213.0f, 520.0f, 332.0f},
                              {213.0f, 520.0f, 227.0f}};

    constexpr float3 ceiling[] {{556.0f, 548.8f, 0.0f},
                                {556.0f, 548.8f, 559.2f},
                                {0.0f, 548.8f, 559.2f},
                                {0.0f, 548.8f, 0.0f}};

    constexpr float3 back_wall[] {{549.6f, 0.0f, 559.2f},
                                  {0.0f, 0.0f, 559.2f},
                                  {0.0f, 548.8f, 559.2f},
                                  {556.0f, 548.8f, 559.2f}};

    constexpr float3 green_wall[] {{0.0f, 0.0f, 559.2f},
                                   {0.0f, 0.0f, 0.0f},
                                   {0.0f, 548.8f, 0.0f},
                                   {0.0f, 548.8f, 559.2f}};

    constexpr float3 red_wall[] {{552.8f, 0.0f, 0.0f},
                                 {549.6f, 0.0f, 559.2f},
                                 {556.0f, 548.8f, 559.2f},
                                 {556.0f, 548.8f, 0.0f}};

    constexpr float3 short_block[] {
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

    constexpr float3 tall_block[] {
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

    return Scene {
        .camera = create_camera(
            {278, 273, -800}, {0, 0, 1}, {0, 1, 0}, 0.035f, 0.025f, 0.025f),
        .triangles =
            {{floor[0], floor[1], floor[2]},
             {floor[0], floor[2], floor[3]},
             {light[0], light[1], light[2]},
             {light[0], light[2], light[3]},
             {ceiling[0], ceiling[1], ceiling[2]},
             {ceiling[0], ceiling[2], ceiling[3]},
             {back_wall[0], back_wall[1], back_wall[2]},
             {back_wall[0], back_wall[2], back_wall[3]},
             {green_wall[0], green_wall[1], green_wall[2]},
             {green_wall[0], green_wall[2], green_wall[3]},
             {red_wall[0], red_wall[1], red_wall[2]},
             {red_wall[0], red_wall[2], red_wall[3]},
             {short_block[0 + 0], short_block[0 + 1], short_block[0 + 2]},
             {short_block[0 + 0], short_block[0 + 2], short_block[0 + 3]},
             {short_block[4 + 0], short_block[4 + 1], short_block[4 + 2]},
             {short_block[4 + 0], short_block[4 + 2], short_block[4 + 3]},
             {short_block[8 + 0], short_block[8 + 1], short_block[8 + 2]},
             {short_block[8 + 0], short_block[8 + 2], short_block[8 + 3]},
             {short_block[12 + 0], short_block[12 + 1], short_block[12 + 2]},
             {short_block[12 + 0], short_block[12 + 2], short_block[12 + 3]},
             {short_block[16 + 0], short_block[16 + 1], short_block[16 + 2]},
             {short_block[16 + 0], short_block[16 + 2], short_block[16 + 3]},
             {tall_block[0 + 0], tall_block[0 + 1], tall_block[0 + 2]},
             {tall_block[0 + 0], tall_block[0 + 2], tall_block[0 + 3]},
             {tall_block[4 + 0], tall_block[4 + 1], tall_block[4 + 2]},
             {tall_block[4 + 0], tall_block[4 + 2], tall_block[4 + 3]},
             {tall_block[8 + 0], tall_block[8 + 1], tall_block[8 + 2]},
             {tall_block[8 + 0], tall_block[8 + 2], tall_block[8 + 3]},
             {tall_block[12 + 0], tall_block[12 + 1], tall_block[12 + 2]},
             {tall_block[12 + 0], tall_block[12 + 2], tall_block[12 + 3]},
             {tall_block[16 + 0], tall_block[16 + 1], tall_block[16 + 2]},
             {tall_block[16 + 0], tall_block[16 + 2], tall_block[16 + 3]}},
        .background_color = {}};
}

float3 sample_pixel(const Scene &scene,
                    int pixel_i,
                    int pixel_j,
                    int image_width,
                    int image_height,
                    Sample_type sample_type,
                    u32 &rng_state)
{
#if 1
    const auto x = (static_cast<float>(pixel_j) + random(rng_state)) /
                       static_cast<float>(image_width) -
                   0.5f;
    const auto y =
        (static_cast<float>(image_height - 1 - pixel_i) + random(rng_state)) /
            static_cast<float>(image_height) -
        0.5f;
#else
    const auto x = (static_cast<float>(pixel_j) + 0.5f) /
                       static_cast<float>(image_width) * 2.0f -
                   1.0f;
    const auto y = (static_cast<float>(image_height - 1 - pixel_i) + 0.5f) /
                       static_cast<float>(image_height) * 2.0f -
                   1.0f;
#endif
    const Ray ray {.origin = scene.camera.position,
                   .direction = normalize(
                       scene.camera.focal_length * scene.camera.direction +
                       x * scene.camera.sensor_width * scene.camera.local_x +
                       y * scene.camera.sensor_height * scene.camera.local_y)};
    const auto payload = intersect(ray, scene.triangles);
    if (payload.primitive_id == 0xffffffffu)
    {
        return scene.background_color;
    }

    switch (sample_type)
    {
    case Sample_type::color:
    {
        return {};
    }
    case Sample_type::albedo:
    {
        return {};
    }
    case Sample_type::normal:
    {
        const auto &triangle = scene.triangles[payload.primitive_id];
        const auto normal =
            normalize(cross(triangle.vertex1 - triangle.vertex0,
                            triangle.vertex2 - triangle.vertex0));
        return (normal + float3 {1.0f, 1.0f, 1.0f}) * 0.5f;
    }
    case Sample_type::barycentric:
    {
        return {payload.u, payload.v, 1.0f - payload.u - payload.v};
    }
    case Sample_type::primitive_id:
    {
        return random_color(payload.primitive_id);
    }
    case Sample_type::material_id:
    {
        return {};
    }
    }

    return {};
}
