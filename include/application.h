#ifndef APPLICATION_H
#define APPLICATION_H

#include "imgui.h"
#include "client.h"
#include "message.h"
#include "imgui_stdlib.h"
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

#define CONTENT_SIZE 67
#define USER_NAME 15

namespace app{
    class Application {
    private:
        Message data;  // 当前消息
        std::string from;   // 发送方用户名
        std::string to;     // 接收方用户名
        std::string content;  // 输入的消息内容
        std::string server_ip;
        std::atomic<bool>& monitor_active;
        std::shared_ptr<std::vector<Message>> messages_arr_ptr;  // 消息列表
        std::shared_ptr<transmit::Client> client_ptr;  // 客户端实例

    public:
        bool show_login = true;  // 是否显示登录框
        bool show_message = false; // 是否显示消息框
        // 构造函数
        Application(std::shared_ptr<std::vector<Message>> m_ptr, std::shared_ptr<transmit::Client> c_ptr,std::atomic<bool>& active):monitor_active(active) {
            messages_arr_ptr = m_ptr;
            client_ptr = c_ptr;
            from.resize(15);
            to.resize(15);
            content.resize(67);
        }

        // 显示登录框
        void showLoginBox() {
            float width = 200.0f;
            float height = 150.0f;
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 2.0f);  // 设置圆角半径为 2
            ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
            {
                ImGui::Begin("Login", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
                
                // 输入框
                ImGui::SetCursorPosX((width - 150.0f) * 0.5f);
                ImGui::SetNextItemWidth(150.0f);
                ImGui::InputTextWithHint("##Input Server IP", u8"服务器IP地址", server_ip.data(), 15);

                ImGui::SetCursorPosX((width - 150.0f) * 0.5f);
                ImGui::SetNextItemWidth(150.0f);
                ImGui::InputTextWithHint("##Input From", u8"你的名字", from.data(), USER_NAME);

                ImGui::SetCursorPosX((width - 150.0f) * 0.5f);
                ImGui::SetNextItemWidth(150.0f);
                ImGui::InputTextWithHint("##Input to", u8"朋友昵称", to.data(), USER_NAME);

                // 设置消息对象
                data.setFrom(from);
                data.setTo(to);
                data.setContent("***Message Init***");

                // 登录按钮
                ImGui::SetCursorPosX((width - 100.0f) * 0.5f);
                ImGui::SetNextItemWidth(40.0f);
                if (ImGui::Button(u8"登录")) {
                    client_ptr->InitAddr(50000,server_ip);
                    client_ptr->connectToServer();
                    client_ptr->sendMessage(data);
                    monitor_active.store(true);
                    show_login = false;
                    show_message = true;
                }

                // 取消按钮
                ImGui::SameLine();
                ImGui::SetCursorPosX((width - 100.0f) * 0.5f + 60);
                ImGui::SetNextItemWidth(40.0f);
                if (ImGui::Button(u8"取消")) {
                    show_login = false;
                }

                ImGui::End();
            }

            ImGui::PopStyleVar();  // 恢复样式
        }

        // 显示消息框
        void showMessageBox() {
            float width = 340.0f;
            float height = 270.0f;

            {
                ImGui::SetNextWindowSize({ width, height }, ImGuiCond_Always);
                ImGui::Begin(to.c_str(),nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

                // 消息滚动区域
                ImGui::SetCursorPosX(20.0f);
                ImGui::BeginChild("ScrollingRegion", { width - 40.0f, 200.0f }, true);

                // 获取当前的滚动位置和最大滚动位置
                float scrollY = ImGui::GetScrollY();
                float scrollMaxY = ImGui::GetScrollMaxY();

                // 检查当前滚动条是否在最底部
                bool isAtBottom = (scrollY >= scrollMaxY - 1.0f);

                // 显示所有消息
                for (const Message& msg : *messages_arr_ptr) {
                    std::string text = msg.getFrom() + " : " + msg.getContent();
                    ImGui::TextWrapped(text.c_str());
                }

                // 如果滚动条在底部，新的内容加载时自动滚动到底部
                if (isAtBottom) {
                    ImGui::SetScrollHereY(1.0f);  // 滚动到最底部
                }

                ImGui::EndChild();

                // 输入框
                ImGui::SetCursorPosX(20.0f);
                ImGui::SetNextItemWidth(300.0f);

                // 确保焦点在输入框，光标始终闪烁
                ImGui::SetKeyboardFocusHere();

                // 显示输入框
                ImGui::InputTextWithHint("##Input Text", u8"Enter 发送", content.data(), CONTENT_SIZE);
    
                // 确保发送按钮能正常响应
                if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                    data.setContent(content);
                    client_ptr->sendMessage(data);
                    std::cout << data.getFrom() << "->" << data.getTo() << " : " << data.getContent() << std::endl;
                    content.clear();  // 发送后清空输入框
                }

                ImGui::End();
            }




        }
    };
}

#endif
