#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"//有这个库inputText就可以用string
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

int showWindow(std::string& message);

int main(){
    //初始化
    if(!glfwInit()){
        return 1;
    }
    //设置版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//隐藏GLFW窗口
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	
    //创建一个GLFW窗口
    GLFWwindow* windows = glfwCreateWindow(1280,720,"hello imgui",nullptr,nullptr);
    if(windows == nullptr){
        return 1;
    }

    //让创建的窗口成为当前的上下文
    glfwMakeContextCurrent(windows);

    //开启垂直同步
    glfwSwapInterval(1);

    //检查imgui版本
    IMGUI_CHECKVERSION();

    //创建imgui上下文
    ImGui::CreateContext();

    //创建io对象
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //设置无窗口化
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport
    io.ConfigViewportsNoAutoMerge = true;                     //可能引起拖动窗口不小心拖动到隐藏的主窗口, 导致子窗口也隐藏的情况.
	
	//更换字体
    io.Fonts->AddFontFromFileTTF("./library/ImGui_glfw_opengl3/front/common.ttf",18,nullptr,io.Fonts->GetGlyphRangesChineseFull());

    //风格设置
    ImGui::StyleColorsLight();
	//在这里可以调很多,具体见demo里的英文
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 4;

	//颜色调整
	//ImVec4* colors = ImGui::GetStyle().Colors;
	
    // 设置渲染器
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(windows, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
	//设置主窗口关闭按钮
	bool show_main_window{true};
    std::string long_text{""};
    std::string input_text;
    input_text.resize(64);
    // char input_text[64];
    //主循环
    while(show_main_window){
        //去除上一帧的颜色
        glClear(GL_COLOR_BUFFER_BIT);
        //遍历事件队列
        glfwPollEvents();

        //如果被小窗口化,则sleep,并且这个循环不进行渲染
        if (glfwGetWindowAttrib(windows, GLFW_ICONIFIED) != 0){
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        //为ImGui新帧做不同环境相关准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //-----------------------------------------------------------------------------------------------//
        //设置最大尺寸和最小尺寸
        ImGui::SetNextWindowSizeConstraints(ImVec2(340, 270), ImVec2(340, 270));
        if(show_main_window){
            //没有折叠框和调整大小
            ImGui::Begin("Client",&show_main_window,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            // 设置文本固定宽度和高度的区域，(0,0)为自适应
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 200), true);
            // 显示长文本，textwrapped可以自动换行
            ImGui::TextWrapped(long_text.c_str());
            ImGui::EndChild();
            // 输入框
            //设置当前光标离左边框20px
            ImGui::SetCursorPosX(20);
            // ImGui::InputTextWithHint("##Input Text","Input Message",input_text.data(),64);
            ImGui::InputTextWithHint("##Input Text","Input Message",&input_text,64);
            ImGui::SameLine();
            //设置当前光标离右边框20px
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Send").x - 20);
            //如果按钮被按下或者enter被按下
            if (ImGui::Button("Send")||ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                long_text += input_text + '\n';
                input_text.clear();
            }
            ImGui::End();
        
		}
        //-----------------------------------------------------------------------------------------------//

        //渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
		//处理 Dear ImGui 中启用了 Viewports 功能时的逻辑
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)//位运算检查 ImGui 的配置标志中是否包含ViewportsEnable 
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();//保存当前的 OpenGL 上下文，以便稍后恢复。
            ImGui::UpdatePlatformWindows();//更新 ImGui 的平台窗口，包括 Viewports 窗口。
            ImGui::RenderPlatformWindowsDefault();//渲染 ImGui 的平台窗口，默认使用 OpenGL 渲染。
            glfwMakeContextCurrent(backup_current_context);//恢复之前保存的 OpenGL 上下文。
        }
        //交换缓存区
        glfwSwapBuffers(windows);
    }
    // 释放资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(windows);
    glfwTerminate();
}

int showWindow(std::string& message){
    //初始化
    if(!glfwInit()){
        return 1;
    }
    //设置版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//隐藏GLFW窗口
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	
    //创建一个GLFW窗口
    GLFWwindow* windows = glfwCreateWindow(1280,720,"hello imgui",nullptr,nullptr);
    if(windows == nullptr){
        return 1;
    }

    //让创建的窗口成为当前的上下文
    glfwMakeContextCurrent(windows);

    //开启垂直同步
    glfwSwapInterval(1);

    //检查imgui版本
    IMGUI_CHECKVERSION();

    //创建imgui上下文
    ImGui::CreateContext();

    //创建io对象
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //设置无窗口化
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport
    io.ConfigViewportsNoAutoMerge = true;                     //可能引起拖动窗口不小心拖动到隐藏的主窗口, 导致子窗口也隐藏的情况.
	
	//更换字体
    io.Fonts->AddFontFromFileTTF("./library/ImGui_glfw_opengl3/front/common.ttf",18,nullptr,io.Fonts->GetGlyphRangesChineseFull());

    //风格设置
    ImGui::StyleColorsLight();
	//在这里可以调很多,具体见demo里的英文
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 4;

	//颜色调整
	//ImVec4* colors = ImGui::GetStyle().Colors;
	
    // 设置渲染器
    const char* glsl_version = "#version 130";
    ImGui_ImplGlfw_InitForOpenGL(windows, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
	//设置主窗口关闭按钮
	bool show_main_window{true};
    std::string long_text{""};
    std::string input_text;
    input_text.resize(64);
    // char input_text[64];
    //主循环
    while(show_main_window){
        //去除上一帧的颜色
        glClear(GL_COLOR_BUFFER_BIT);
        //遍历事件队列
        glfwPollEvents();

        //如果被小窗口化,则sleep,并且这个循环不进行渲染
        if (glfwGetWindowAttrib(windows, GLFW_ICONIFIED) != 0){
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
        //为ImGui新帧做不同环境相关准备
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //-----------------------------------------------------------------------------------------------//
        //设置最大尺寸和最小尺寸
        ImGui::SetNextWindowSizeConstraints(ImVec2(340, 270), ImVec2(340, 270));
        if(show_main_window){
            //没有折叠框和调整大小
            ImGui::Begin("Client",&show_main_window,ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            // 设置文本固定宽度和高度的区域，(0,0)为自适应
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 200), true);
            // 显示长文本，textwrapped可以自动换行
            ImGui::TextWrapped(long_text.c_str());
            ImGui::EndChild();
            // 输入框
            //设置当前光标离左边框20px
            ImGui::SetCursorPosX(20);
            // ImGui::InputTextWithHint("##Input Text","Input Message",input_text.data(),64);
            ImGui::InputTextWithHint("##Input Text","Input Message",&input_text,64);
            ImGui::SameLine();
            //设置当前光标离右边框20px
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Send").x - 20);
            //如果按钮被按下或者enter被按下
            if (ImGui::Button("Send")||ImGui::IsKeyPressed(ImGuiKey_Enter))
            {
                long_text += input_text + '\n';
                input_text.clear();
            }
            ImGui::End();
        
		}
        //-----------------------------------------------------------------------------------------------//

        //渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
		//处理 Dear ImGui 中启用了 Viewports 功能时的逻辑
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)//位运算检查 ImGui 的配置标志中是否包含ViewportsEnable 
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();//保存当前的 OpenGL 上下文，以便稍后恢复。
            ImGui::UpdatePlatformWindows();//更新 ImGui 的平台窗口，包括 Viewports 窗口。
            ImGui::RenderPlatformWindowsDefault();//渲染 ImGui 的平台窗口，默认使用 OpenGL 渲染。
            glfwMakeContextCurrent(backup_current_context);//恢复之前保存的 OpenGL 上下文。
        }
        //交换缓存区
        glfwSwapBuffers(windows);
    }
    // 释放资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(windows);
    glfwTerminate();
    return 0;
}