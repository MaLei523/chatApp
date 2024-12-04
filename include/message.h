#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <cstring>

class Message {
private:
    char from[16];    // 发送方，最大15字符 + '\0'
    char to[16];      // 接收方，最大15字符 + '\0'
    char content[68]; // 内容，最大67字符 + '\0'

public:
    // 设置发送方
    void setFrom(const std::string& str_from) {
        if (str_from.size() > 15) {
            std::cerr << "Error: 'from' exceeds 15 characters." << std::endl;
            return;
        }
        std::strncpy(from, str_from.c_str(), 15);
        from[15] = '\0'; // 确保以 '\0' 结尾
    }

    // 设置接收方
    void setTo(const std::string& str_to) {
        if (str_to.size() > 15) {
            std::cerr << "Error: 'to' exceeds 15 characters." << std::endl;
            return;
        }
        std::strncpy(to, str_to.c_str(), 15);
        to[15] = '\0'; // 确保以 '\0' 结尾
    }

    // 设置内容
    void setContent(const std::string& str_content) {
        if (str_content.size() > 67) {
            std::cerr << "Error: 'content' exceeds 67 characters." << std::endl;
            return;
        }
        std::strncpy(content, str_content.c_str(), sizeof(content) - 1);
        content[sizeof(content) - 1] = '\0'; // 确保以 '\0' 结尾
    }

    // 获取发送方
    std::string getFrom() const {
        return std::string(from);
    }

    // 获取接收方
    std::string getTo() const {
        return std::string(to);
    }

    // 获取内容
    std::string getContent() const {
        return std::string(content);
    }

    // 序列化
    void serialize(char* buffer) const {
        std::memcpy(buffer, this, sizeof(Message));
    }

    // 反序列化
    void deserialize(const char* buffer) {
        std::memcpy(this, buffer, sizeof(Message));
    }
    void swapFromTo(){
        std::swap(from,to);
    }
};

#endif
