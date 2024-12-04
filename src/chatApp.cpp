#include <memory>
#include <thread>
#include <atomic>
#include "application.h"
#include "client.h"
#include "gui.h"
#include "message.h"

int main(){
    //原子量
    std::atomic<bool> monitor_active(false);
    //四个指针
    std::shared_ptr<std::vector<Message>> messages= std::make_shared<std::vector<Message>>();

    std::shared_ptr<transmit::Client> transmit = std::make_shared<transmit::Client>();

    std::thread monitor_thread([&messages,&transmit,&monitor_active](){transmit->monitor(messages,monitor_active);});

    std::shared_ptr<app::Application> app = std::make_shared<app::Application>(messages,transmit,monitor_active);

    std::shared_ptr<gui::MainGui> gui = std::make_shared<gui::MainGui>();


    
    gui->init();
    gui->run(app);
    
    return 0;
}