#ifndef GUI_H
#define GUI_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h" // 有这个库inputText就可以用string
#include "application.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <iostream>
#include <string>
#include <memory>

namespace gui{
    class MainGui {
    private:
        GLFWwindow* window = nullptr;
        std::string glsl_version = "#version 130";
        ImVec4 clear_color = {0.81f, 0.94f, 0.75f, 1.00f};//glfw背景色
        bool show_demo_window = true;
        bool show_another_window = false;

    public:

        MainGui(){}

        // Initialize GLFW, ImGui, and the OpenGL context
        bool init() {
            // Initialize GLFW
            if (!glfwInit())
                return false;


            // Setup window hints
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            // Create GLFW window
            window = glfwCreateWindow(500, 500, "chatApp version-1.00", nullptr, nullptr);
            if (window == nullptr)
                return false;

            glfwMakeContextCurrent(window);
            glfwSwapInterval(1); // Enable vsync

            // Initialize ImGui
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;

            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
            // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
            // io.ConfigViewportsNoAutoMerge = true;  
            //更换字体
            io.Fonts->AddFontFromFileTTF("common.ttf",18,nullptr,io.Fonts->GetGlyphRangesChineseFull());
            
            // Setup Dear ImGui style
            ImGui::StyleColorsLight();

            // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
            ImGuiStyle& style = ImGui::GetStyle();
            style.ChildRounding = 2.0f;



            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version.c_str());

            return true;
        }

        // Run the main application loop
        void run(std::shared_ptr<app::Application> app) {
            while (!glfwWindowShouldClose(window)) {
                glfwPollEvents();

                if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
                    ImGui_ImplGlfw_Sleep(10);
                    continue;
                }

                // Start the Dear ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                //-----//
                if(app->show_login){
                    app->showLoginBox();
                }
                if(app->show_message){
                    app->showMessageBox();
                }

                //-----//

                // Rendering
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                // Update and Render additional Platform Windows
                if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                    GLFWwindow* backup_current_context = glfwGetCurrentContext();
                    ImGui::UpdatePlatformWindows();
                    ImGui::RenderPlatformWindowsDefault();
                    glfwMakeContextCurrent(backup_current_context);
                }

                glfwSwapBuffers(window);
            }
        }

        // Cleanup resources
        void cleanup() {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
            glfwTerminate();
        }
        // Destructor to handle cleanup
        ~MainGui() {
            cleanup();
        }
    };
}


#endif 


