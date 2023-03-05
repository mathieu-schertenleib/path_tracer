#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

namespace
{

void glfw_error_callback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

struct Pixel
{
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
};

[[nodiscard]] inline constexpr float random(std::uint32_t &state) noexcept
{
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return static_cast<float>(state) /
           static_cast<float>(std::numeric_limits<std::uint32_t>::max());
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

    const auto glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(
        glfwGetProcAddress("glGenFramebuffers"));
    assert(glGenFramebuffers);
    const auto glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(
        glfwGetProcAddress("glBindFramebuffer"));
    assert(glBindFramebuffer);
    const auto glFramebufferTexture2D =
        reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(
            glfwGetProcAddress("glFramebufferTexture2D"));
    assert(glFramebufferTexture2D);
    const auto glDeleteFramebuffers =
        reinterpret_cast<PFNGLDELETEFRAMEBUFFERSPROC>(
            glfwGetProcAddress("glDeleteFramebuffers"));
    assert(glDeleteFramebuffers);
    const auto glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(
        glfwGetProcAddress("glBlitFramebuffer"));
    assert(glBlitFramebuffer);

    constexpr int framebuffer_width {160};
    constexpr int framebuffer_height {90};
    std::vector<Pixel> pixels(
        static_cast<std::size_t>(framebuffer_width * framebuffer_height));

    std::uint32_t rng_state {2345678};

    GLuint texture {};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    GLuint framebuffer {};
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(
        GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        for (auto &pixel : pixels)
        {
            pixel.r = static_cast<std::uint8_t>(random(rng_state) * 255.0f);
            pixel.g = static_cast<std::uint8_t>(random(rng_state) * 255.0f);
            pixel.b = static_cast<std::uint8_t>(random(rng_state) * 255.0f);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Performance");
        ImGui::Text("%.2f ms/frame, %.1f fps",
                    static_cast<double>(1000.0f / ImGui::GetIO().Framerate),
                    static_cast<double>(ImGui::GetIO().Framerate));
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
                     framebuffer_width,
                     framebuffer_height,
                     0,
                     GL_RGB,
                     GL_UNSIGNED_BYTE,
                     pixels.data());

        glBlitFramebuffer(0,
                          0,
                          framebuffer_width,
                          framebuffer_height,
                          0,
                          0,
                          window_width,
                          window_height,
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
