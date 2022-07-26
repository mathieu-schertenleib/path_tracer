#include "definitions.hpp"
#include "math.hpp"
#include "random.hpp"
#include "render.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wnull-dereference"
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

namespace
{

struct Pixel
{
    u8 r;
    u8 g;
    u8 b;
};

void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

[[nodiscard]] FORCE_INLINE constexpr u8 f32_to_u8(f32 c) noexcept
{
    return static_cast<u8>(math::clamp(c, 0.0f, 1.0f) * 255.0f);
}

[[nodiscard]] FORCE_INLINE f32 linear_to_srgb(f32 c)
{
    if (c <= 0.0031308f)
    {
        return 12.92f * c;
    }
    return 1.055f * math::pow(c, 1.0f / 2.4f) - 0.055f;
}

} // namespace

int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    constexpr auto glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    auto *const window =
        glfwCreateWindow(1280, 720, "Path tracer", nullptr, nullptr);
    if (window == nullptr)
    {
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    constexpr int image_width {256};
    constexpr int image_height {256};
    constexpr auto image_size {
        static_cast<std::size_t>(image_width * image_height)};
    std::vector<Pixel> pixel_buffer(image_size);
    std::vector<f32v3> accumulation_buffer(image_size);

    GLuint texture {};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const auto scene = cornell_box();

    int samples {0};
    int samples_per_frame {1};
    int total_samples {1};

    char image_filename[256] {};

    auto rng_state = seed(std::random_device {}());
    if (rng_state == 0)
    {
        rng_state = 123456789;
    }
    auto color_rng_state = rng_state;

    Sample_type sample_type {Sample_type::primitive_id};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Settings"))
        {
            ImGui::Text("%.2f ms/frame, %.1f fps",
                        static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
                        static_cast<double>(ImGui::GetIO().Framerate));

            ImGui::Text("%lld triangles", scene.triangles.size());

            ImGui::Text("%d samples", samples);

            // ImGui::SliderInt("Samples per frame", &samples_per_frame, 1, 8);

            ImGui::InputInt("Total samples", &total_samples);
            bool reset_samples {false};
            if (total_samples < 1)
            {
                total_samples = 1;
            }
            else if (samples > total_samples)
            {
                reset_samples = true;
            }

            if (ImGui::Button("Reset samples"))
            {
                reset_samples = true;
            }

            constexpr const char *sample_types[] {"color",
                                                  "albedo",
                                                  "normal",
                                                  "barycentric",
                                                  "primitive_id",
                                                  "material_id"};
            auto sample_type_int = static_cast<int>(sample_type);
            if (ImGui::Combo("Sample type",
                             &sample_type_int,
                             sample_types,
                             static_cast<int>(std::size(sample_types))))
            {
                reset_samples = true;
            }
            sample_type = static_cast<Sample_type>(sample_type_int);

            if (ImGui::Button("Change colors"))
            {
                color_rng_state = rng_state;
                reset_samples = true;
            }

            if (reset_samples)
            {
                std::fill(accumulation_buffer.begin(),
                          accumulation_buffer.end(),
                          f32v3 {});
                samples = 0;
            }

            ImGui::InputText(
                "PNG file name", image_filename, sizeof(image_filename));
            if (ImGui::Button("Store to PNG") &&
                std::strlen(image_filename) > 0)
            {
                if (stbi_write_png(image_filename,
                                   image_width,
                                   image_height,
                                   3,
                                   pixel_buffer.data(),
                                   image_width * 3))
                {
                    std::cout << "Image written as \"" << image_filename
                              << "\"\n";
                }
                else
                {
                    std::cerr << "Failed to write PNG image \""
                              << image_filename << "\"\n";
                }
            }
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2 {0.0f, 0.0f});
        if (ImGui::Begin("Viewport"))
        {
            const auto [viewport_width, viewport_height] =
                ImGui::GetContentRegionAvail();

            constexpr auto image_aspect_ratio =
                static_cast<f32>(image_width) / static_cast<f32>(image_height);
            const auto viewport_aspect_ratio = viewport_width / viewport_height;
            ImVec2 image_displayed_size {};
            if (viewport_aspect_ratio >= image_aspect_ratio)
            {
                image_displayed_size.x = viewport_height * image_aspect_ratio;
                image_displayed_size.y = viewport_height;
            }
            else
            {
                image_displayed_size.x = viewport_width;
                image_displayed_size.y = viewport_width / image_aspect_ratio;
            }

            ImGui::Image(
                reinterpret_cast<void *>(static_cast<std::uintptr_t>(texture)),
                image_displayed_size);
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Render();

        for (int s {}; s < samples_per_frame && samples < total_samples; ++s)
        {
            for (int i {}; i < image_height; ++i)
            {
                for (int j {}; j < image_width; ++j)
                {
                    const auto sample_color = sample_pixel(scene,
                                                           i,
                                                           j,
                                                           image_width,
                                                           image_height,
                                                           sample_type,
                                                           rng_state,
                                                           color_rng_state);
                    const auto pixel_index =
                        static_cast<std::size_t>(i) *
                            static_cast<std::size_t>(image_width) +
                        static_cast<std::size_t>(j);
                    accumulation_buffer[pixel_index] += sample_color;
                }
            }
            ++samples;
        }

        for (std::size_t i {}; i < image_size; ++i)
        {
            const auto color =
                accumulation_buffer[i] / static_cast<f32>(samples);
            pixel_buffer[i] = {f32_to_u8(linear_to_srgb(color.x)),
                               f32_to_u8(linear_to_srgb(color.y)),
                               f32_to_u8(linear_to_srgb(color.z))};
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     image_width,
                     image_height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     pixel_buffer.data());

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteTextures(1, &texture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
