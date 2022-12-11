#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "ImNodes.h"
#include "ImNodesEz.h"

#include <cstddef>
#include <cstdlib>
#include <stdio.h>
#include <ctime>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

class ApplicationTemplate {
public:
        ApplicationTemplate() {
                glfwSetErrorCallback(glfw_error_callback);
                if (!glfwInit()) std::exit(1);

                // Decide GL+GLSL versions
                #if defined(IMGUI_IMPL_OPENGL_ES2)
                        // GL ES 2.0 + GLSL 100
                        const char* glsl_version = "#version 100";
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
                        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
                #elif defined(__APPLE__)
                        // GL 3.2 + GLSL 150
                        const char* glsl_version = "#version 150";
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
                        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
                        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
                #else
                        // GL 3.0 + GLSL 130
                        const char* glsl_version = "#version 130";
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
                        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
                        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
                #endif

                // Create window with graphics context
                glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
                window = glfwCreateWindow(1024, 768, "B-tree", NULL, NULL);
                if (window == NULL) std::exit(1);

                glfwSetWindowPos(window, 448, 156);

                glfwMakeContextCurrent(window);
                glfwSwapInterval(1); // Enable vsync

                // Setup Dear ImGui context
                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImPlot::CreateContext();

                ImGuiIO& io = ImGui::GetIO(); (void)io;
                io.Fonts->AddFontFromFileTTF(
                        "/home/_3982580943570984/JetBrains Mono Regular Nerd Font Complete Mono.ttf",
                        22.f,
                        NULL,
                        io.Fonts->GetGlyphRangesCyrillic());
                //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
                //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

                // Setup Dear ImGui style
                ImGui::StyleColorsClassic();
                //ImGui::StyleColorsLight();

                // Setup Platform/Renderer backends
                ImGui_ImplGlfw_InitForOpenGL(window, true);
                ImGui_ImplOpenGL3_Init(glsl_version);

        }

        virtual ~ApplicationTemplate() {
                // Cleanup
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImPlot::DestroyContext();
                ImGui::DestroyContext();
                glfwDestroyWindow(window);
                glfwTerminate();
        }

        void Run() {

                srand(time(NULL));

                StartUp();
                while (!glfwWindowShouldClose(window)) {
                        glfwPollEvents();
                        ImGui_ImplOpenGL3_NewFrame();
                        ImGui_ImplGlfw_NewFrame();
                        ImGui::NewFrame();

                        Update();

                        // Rendering
                        ImGui::Render();
                        int display_w, display_h;
                        glfwGetFramebufferSize(window, &display_w, &display_h);
                        glViewport(0, 0, display_w, display_h);
                        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                        glClear(GL_COLOR_BUFFER_BIT);
                        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                        glfwSwapBuffers(window);

                }
        }

        virtual void StartUp() = 0;
        virtual void Update() = 0;
private:
        GLFWwindow* window = nullptr;
public:
        ImVec4 clear_color = ImVec4(255.f/255, 0.f/255, 255.f/255, 1.00f);
};
