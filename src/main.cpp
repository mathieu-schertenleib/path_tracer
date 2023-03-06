#include "random.hpp"
#include "vec3.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

namespace
{

struct Pixel
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
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

    ImGui::GetIO().IniFilename = nullptr;
    ImGui::GetIO().LogFilename = nullptr;

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

    constexpr int buffer_width {160};
    constexpr int buffer_height {90};
    constexpr auto buffer_size {
        static_cast<std::size_t>(buffer_width * buffer_height)};
    std::vector<Pixel> pixel_buffer(buffer_size);
    std::vector<vec3> accumulation_buffer(buffer_size);

    std::uint32_t rng_state {123456789};

    GLuint texture {};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint framebuffer {};
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    int samples {1};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (std::size_t i {}; i < buffer_size; ++i)
        {
            const vec3 sample_color {
                random(rng_state), random(rng_state), random(rng_state)};
            accumulation_buffer[i] += sample_color;
            const auto color {accumulation_buffer[i] /
                              static_cast<float>(samples)};
            pixel_buffer[i] = {static_cast<std::uint8_t>(color.x * 255.0f),
                               static_cast<std::uint8_t>(color.y * 255.0f),
                               static_cast<std::uint8_t>(color.z * 255.0f)};
        }
        ++samples;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Performance");
        ImGui::Text("%.2f ms/frame, %.1f fps",
                    static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
                    static_cast<double>(ImGui::GetIO().Framerate));
        ImGui::Text("%d samples", samples);
        ImGui::End();

        ImGui::Render();

        int window_width {};
        int window_height {};
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glViewport(0, 0, window_width, window_height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB,
                     buffer_width,
                     buffer_height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     pixel_buffer.data());

        constexpr auto buffer_aspect_ratio {static_cast<float>(buffer_width) /
                                            static_cast<float>(buffer_height)};
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
                          buffer_width,
                          buffer_height,
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
