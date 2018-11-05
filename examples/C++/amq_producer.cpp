#include "amq_producer.h"
#include "amq_config.h"

using namespace cms;

amq_producer::amq_producer(
    bool pUseQueue, 
    bool pSessionTransacted) : 
    _connection(nullptr),
    _session(nullptr),
    _destination(nullptr),
    _producer(nullptr),
    _use_quque(pUseQueue),
    _session_transacted(pSessionTransacted)
{
}

amq_producer::~amq_producer()
{
    close();
}

void amq_producer::run()
{
    try 
    {
        //create a connection factory
        auto _connection_factory = ConnectionFactory::createCMSConnectionFactory(BROKER_URI);
        //create a connection
        this->_connection = _connection_factory->createConnection(USER_NAME, PASS_WORD);
        this->_connection->start();
            
        // create a Session
        if (this->_session_transacted) 
        {
            this->_session = this->_connection->createSession(Session::SESSION_TRANSACTED);
        } 
        else 
        {
            this->_session = this->_connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // create the destination (Topic or Queue)
        if (this->_use_quque) 
        {
            this->_destination = this->_session->createQueue(PRODUCER_QUEUE_NAME);
        }
        else
        {
            this->_destination = this->_session->createTopic(PRODUCER_TOPIC_NAME);
        }

        // create a MessageProducer from the Session to the Topic or Queue
        this->_producer = this->_session->createProducer(this->_destination);
        this->_producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

        this->_thread_handle.add_job([this]()
        {
            for(;;)
            {
                //wait on signal to send message
                Message _message;
                this->_messages.pop(_message);

                if(!_message.msg.empty())
                {
                    //send message
                    auto _text_message = this->_session->createTextMessage(_message.msg);
                    if (!_text_message)
                    {
                        //could not allocate memory for text message
                        _last_error = "could not allocate memory for TextMessage";
                    }
                    else
                    {
                        //set property
                        _text_message->setIntProperty("Integer", _message.priority);
                        //send text message
                        this->_producer->send(_text_message);
                       
                        //release text message
                        _text_message->clearBody();
                        delete _text_message;
                        _text_message = nullptr;
                    }
               }
            }
        });
    }
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_producer::run_async()";
    }
}

void amq_producer::send_message(const std::string& pMessage, const int& pPriority)
{
    Message _message;
    _message.msg = pMessage;
    _message.priority = pPriority;    
    this->_messages.push(_message);
}

void amq_producer::close()
{    
    //destroy resources
    try 
    {
        if (this->_connection) 
        {
            this->_connection->close();
            delete this->_connection;
            this->_connection = nullptr;
        }
        if(this->_destination)
        {
            delete this->_destination;
            this->_destination = nullptr;
        }
        if(this->_producer)
        {
            delete this->_producer;
            this->_producer = nullptr;
        }
        if(this->_session)
        {
            delete this->_session;
            this->_session = nullptr;
        }
        if(this->_connection)
        {
            delete this->_connection;
            this->_connection = nullptr;
        }
    }
    catch (CMSException& e) 
    {
        _last_error = "exception occurred: " + 
            e.getStackTraceString() + 
            " .trace info: amq_producer::close()";
    }
}

const std::string amq_producer::get_last_error() const
{
    return this->_last_error;
}
