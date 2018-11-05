#include <iostream>
#include "amq_consumer.h"
#include "amq_producer.h"

static wolf::system::w_concurrent_queue<std::string> _queue;

int main(int, char**) 
{
    library::ActiveMQCPP::initializeLibrary();

    amq_consumer _consumer;
    amq_producer _producer;

    _consumer.on_message_received = [](const std::string pMessage) -> void
    {
        _queue.push(pMessage);
    };
    wolf::system::w_thread _thread;
    _thread.add_job([&]()
    {
        std::string _message;
        for(;;)
        {
            if(!_queue.safe_size())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }
            _queue.pop(_message);
            if(_message.empty()) continue;

			std::string _response = "make your own response";
			int _priority = 0;
           _producer.send_message(_response, _priority);
        }
    });

    _consumer.run();
    _producer.run();

    std::getchar();
    _consumer.close();
    _producer.close();

    activemq::library::ActiveMQCPP::shutdownLibrary();

    return EXIT_SUCCESS;
}
