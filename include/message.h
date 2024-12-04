#ifndef MESSAGE_H
#define MESSAGE_H

#include <iostream>
#include <cstring>

class Message {
private:
    char from[16];    // ���ͷ������15�ַ� + '\0'
    char to[16];      // ���շ������15�ַ� + '\0'
    char content[68]; // ���ݣ����67�ַ� + '\0'

public:
    // ���÷��ͷ�
    void setFrom(const std::string& str_from) {
        if (str_from.size() > 15) {
            std::cerr << "Error: 'from' exceeds 15 characters." << std::endl;
            return;
        }
        std::strncpy(from, str_from.c_str(), 15);
        from[15] = '\0'; // ȷ���� '\0' ��β
    }

    // ���ý��շ�
    void setTo(const std::string& str_to) {
        if (str_to.size() > 15) {
            std::cerr << "Error: 'to' exceeds 15 characters." << std::endl;
            return;
        }
        std::strncpy(to, str_to.c_str(), 15);
        to[15] = '\0'; // ȷ���� '\0' ��β
    }

    // ��������
    void setContent(const std::string& str_content) {
        if (str_content.size() > 67) {
            std::cerr << "Error: 'content' exceeds 67 characters." << std::endl;
            return;
        }
        std::strncpy(content, str_content.c_str(), sizeof(content) - 1);
        content[sizeof(content) - 1] = '\0'; // ȷ���� '\0' ��β
    }

    // ��ȡ���ͷ�
    std::string getFrom() const {
        return std::string(from);
    }

    // ��ȡ���շ�
    std::string getTo() const {
        return std::string(to);
    }

    // ��ȡ����
    std::string getContent() const {
        return std::string(content);
    }

    // ���л�
    void serialize(char* buffer) const {
        std::memcpy(buffer, this, sizeof(Message));
    }

    // �����л�
    void deserialize(const char* buffer) {
        std::memcpy(this, buffer, sizeof(Message));
    }
    void swapFromTo(){
        std::swap(from,to);
    }
};

#endif
