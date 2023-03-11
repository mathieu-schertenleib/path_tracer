#include "definitions.hpp"
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
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
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

#define LOAD_FUNCTION(name, type)                                              \
    const auto name = reinterpret_cast<type>(glfwGetProcAddress(#name))

    LOAD_FUNCTION(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
    LOAD_FUNCTION(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
    LOAD_FUNCTION(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
    LOAD_FUNCTION(glBlitFramebuffer, PFNGLBLITFRAMEBUFFERPROC);
    LOAD_FUNCTION(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC);

#undef LOAD_FUNCTION

    constexpr int image_width {256};
    constexpr int image_height {256};
    constexpr auto image_size {
        static_cast<std::size_t>(image_width * image_height)};
    std::vector<Pixel> pixel_buffer(image_size);
    std::vector<float3> accumulation_buffer(image_size);

    GLuint texture {};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint framebuffer {};
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

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

    Sample_type sample_type {Sample_type::primitive_id};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (ImGui::Begin("Performance"))
        {
            ImGui::Text("%.2f ms/frame, %.1f fps",
                        static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
                        static_cast<double>(ImGui::GetIO().Framerate));

            ImGui::Text("%d samples", samples);

            ImGui::SliderInt("Samples per frame", &samples_per_frame, 1, 100);

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

            constexpr const char *sample_types[5] {
                "radiance", "base_color", "primitive_id", "material_id", "uv"};
            auto sample_type_int = static_cast<int>(sample_type);
            if (ImGui::Combo("Sample type",
                             &sample_type_int,
                             sample_types,
                             std::size(sample_types)))
            {
                reset_samples = true;
            }
            sample_type = static_cast<Sample_type>(sample_type_int);

            if (reset_samples)
            {
                std::fill(accumulation_buffer.begin(),
                          accumulation_buffer.end(),
                          float3 {});
                samples = 0;
            }

            ImGui::InputText(
                "PNG file name", image_filename, sizeof(image_filename));
            if (ImGui::Button("Store to PNG") &&
                std::strlen(image_filename) > 0)
            {
                stbi_flip_vertically_on_write(true);
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
                                                           rng_state);
                    const auto pixel_index =
                        static_cast<std::size_t>(i) *
                            static_cast<std::size_t>(image_width) +
                        static_cast<std::size_t>(j);
                    accumulation_buffer[pixel_index] += sample_color;
                }
            }
            ++samples;
        }

        for (int i {}; i < image_height; ++i)
        {
            for (int j {}; j < image_width; ++j)
            {
                const auto buffer_index =
                    static_cast<std::size_t>(i) *
                        static_cast<std::size_t>(image_width) +
                    static_cast<std::size_t>(j);
                const auto color = accumulation_buffer[buffer_index] /
                                   static_cast<float>(samples);
                const auto pixel_index =
                    static_cast<std::size_t>(image_height - 1 - i) *
                        static_cast<std::size_t>(image_width) +
                    static_cast<std::size_t>(j);
                pixel_buffer[pixel_index] = {static_cast<u8>(color.x * 255.0f),
                                             static_cast<u8>(color.y * 255.0f),
                                             static_cast<u8>(color.z * 255.0f)};
            }
        }

        int window_width {};
        int window_height {};
        glfwGetFramebufferSize(window, &window_width, &window_height);
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

        constexpr auto buffer_aspect_ratio {static_cast<float>(image_width) /
                                            static_cast<float>(image_height)};
        const auto window_aspect_ratio {static_cast<float>(window_width) /
                                        static_cast<float>(window_height)};
        int displayed_x0;
        int displayed_y0;
        int displayed_x1;
        int displayed_y1;
        if (window_aspect_ratio >= buffer_aspect_ratio)
        {
            const auto displayed_width {static_cast<int>(
                static_cast<float>(window_height) * buffer_aspect_ratio)};
            const auto displayed_height {window_height};
            displayed_x0 = (window_width - displayed_width) / 2;
            displayed_y0 = 0;
            displayed_x1 = displayed_x0 + displayed_width;
            displayed_y1 = displayed_y0 + displayed_height;
        }
        else
        {
            const auto displayed_width {window_width};
            const auto displayed_height {static_cast<int>(
                static_cast<float>(window_width) / buffer_aspect_ratio)};
            displayed_x0 = 0;
            displayed_y0 = (window_height - displayed_height) / 2;
            displayed_x1 = displayed_x0 + displayed_width;
            displayed_y1 = displayed_y0 + displayed_height;
        }

        glBlitFramebuffer(0,
                          0,
                          image_width,
                          image_height,
                          displayed_x0,
                          displayed_y0,
                          displayed_x1,
                          displayed_y1,
                          GL_COLOR_BUFFER_BIT,
                          GL_NEAREST);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &texture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
