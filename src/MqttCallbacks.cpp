#include "MqttCallbacks.hpp"

std::string MqttCallbacks::getTokenTypeStr(mqtt::token::Type type){
    switch(type){
        case mqtt::token::CONNECT:{
            return "Connect";
        }
        case mqtt::token::SUBSCRIBE:{
            return "Subscribe";
        }
        case mqtt::token::PUBLISH:{
            return "Publish";
        }
        case mqtt::token::UNSUBSCRIBE:{
            return "Unsubscribe";
        }
        case mqtt::token::DISCONNECT:{
            return "Disconnect";
        }
        default:
            return "";
    }
}

bool MqttCallbacks::isMqttTopicIncluded(const std::string& topic, const std::string& filter) {
    size_t tlen = topic.length();
    size_t flen = filter.length();
    size_t tidx = 0;
    size_t fidx = 0;

    while (tidx < tlen && fidx < flen) {
        if (filter[fidx] == '+') {
            // Single level wildcard, skip to the next level in topic
            ++fidx;
            while (tidx < tlen && topic[tidx] != '/')
                ++tidx;
        } else if (filter[fidx] == '#') {
            // Multi level wildcard, match remaining levels in topic
            if (fidx + 1 == flen) // # is at the end of the filter
                return true;
            while (tidx < tlen && topic[tidx] != '/')
                ++tidx;
            ++fidx;
        } else {
            // Normal character, must match exactly
            if (topic[tidx] != filter[fidx])
                return false;
            ++tidx;
            ++fidx;
        }
    }

    // If both topic and filter have been fully processed, they match
    return tidx == tlen && fidx == flen;
}

void MqttCallbacks::reconnect() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    try {
        _mqttClient.reconnect();
    } catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
    }
}

void MqttCallbacks::on_failure(const mqtt::token& tok) {
    //std::cout << "Action failed for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
}

void MqttCallbacks::on_success(const mqtt::token& tok) {
    //std::cout << "Action succeeded for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
}

void MqttCallbacks::delivery_complete(mqtt::delivery_token_ptr tok) {
    PublishResult result;
    if(tok->get_return_code() == MQTTASYNC_SUCCESS){
         /* std::cout << "Message delivery complete for MQTT client " << _mqttClient.get_client_id() << ": " << std::to_string(tok->get_message_id()) 
            << std::endl;  */
        result = PUBLISH_SUCCESS;
    } else {
        /* std::cout << "Message delivery failed for MQTT client " << _mqttClient.get_client_id() << ": " << std::to_string(tok->get_message_id()) 
            << "Return code: "<< tok->get_return_code() << std::endl; */
        result = PUBLISH_FAILURE;
    }
    if(_onPublishResultCallback){
        _onPublishResultCallback(result, tok->get_message_id(), tok->get_message());
    }
}

void MqttCallbacks::connected(const std::string& cause) {
    //std::cout << "Connection success for MQTT client " << _mqttClient.get_client_id() << std::endl;
    for(auto messageHandler : messageHandlers){
        _mqttClient.subscribe(std::get<0>(messageHandler), 0, nullptr, *this);
    }
    if(_onConnectCallback){
        _onConnectCallback();
    }
}

void MqttCallbacks::connection_lost(const std::string& cause){
    //std::cout << "Connection lost for MQTT client" << this->_mqttClient.get_client_id() << std::endl;
    if (!cause.empty()){
        //std::cout << "Cause: " << cause << std::endl;
    }
    if(_onDisconnectCallback){
        _onDisconnectCallback();
    }
}

void MqttCallbacks::messageArrivedHandler(mqtt::const_message_ptr msg){
    for(auto messageHandler : messageHandlers){
        if(isMqttTopicIncluded(msg->get_topic(), std::get<0>(messageHandler))){
            std::string response = std::get<1>(messageHandler)(msg->get_topic(), msg->to_string());
            mqtt::properties msgProps = msg->get_properties();
            if(msgProps.contains(mqtt::property::code::RESPONSE_TOPIC)){
                const mqtt::property& responseTopicProp = msgProps.get(mqtt::property::code::RESPONSE_TOPIC);
                const std::string& responseTopic = mqtt::get<std::string>(responseTopicProp);
                _mqttClient.publish(responseTopic, response);
            }
        }
    }
}

// Enqueue task for execution by the thread pool 
void MqttCallbacks::enqueue(mqtt::const_message_ptr msg) {
    {
        std::unique_lock<std::mutex> lock(queueMutex); 
        msgQueue.emplace(std::move(msg)); 
    }
    conditionVariable.notify_one(); 
}

void MqttCallbacks::message_arrived(mqtt::const_message_ptr msg){
    //std::cout << "Message arrived from broker" << std::endl;
    //std::cout << "topic: '" << msg->get_topic() << "'" << std::endl;
    //std::cout << "payload: '" << msg->to_string() << std::endl;
    enqueue(msg);
}

MqttCallbacks::MqttCallbacks(mqtt::async_client& mqttClient, mqtt::connect_options& connOpts, int numRcvHandlerTasks)
    : _mqttClient(mqttClient), _connOpts(connOpts) {
        for (size_t i = 0; i < numRcvHandlerTasks; ++i) { 
            rcvHandlersThreads.emplace_back([this, i] { 
                while (true) { 
                    mqtt::const_message_ptr msg; 
                    // The reason for putting the below code 
                    // here is to unlock the queue before 
                    // executing the task so that other 
                    // threads can perform enqueue tasks 
                    {
                        // Locking the queue so that data 
                        // can be shared safely 
                        std::unique_lock<std::mutex> lock(queueMutex); 

                        // Waiting until there is a task to 
                        // execute or the pool is stopped 
                        conditionVariable.wait(lock, [this] { 
                            return !msgQueue.empty() || stopExecution; 
                        }); 

                        // exit the thread in case the pool 
                        // is stopped and there are no tasks 
                        if (stopExecution && msgQueue.empty()) { 
                            return; 
                        } 

                        // Get the next msg from the queue 
                        msg = move(msgQueue.front()); 
                        msgQueue.pop(); 
                    }

                    // std::cout << "Received msg on topic " << msg->get_topic() << "(worker thread " << i  << ")" <<std::endl;
                    try{
                        for(auto messageHandler : messageHandlers){
                            if(isMqttTopicIncluded(msg->get_topic(), std::get<0>(messageHandler))){
                                std::string response = std::get<1>(messageHandler)(msg->get_topic(), msg->to_string());
                                mqtt::properties msgProps = msg->get_properties();
                                if(msgProps.contains(mqtt::property::code::RESPONSE_TOPIC)){
                                    const mqtt::property& responseTopicProp = msgProps.get(mqtt::property::code::RESPONSE_TOPIC);
                                    const std::string& responseTopic = mqtt::get<std::string>(responseTopicProp);
                                    _mqttClient.publish(responseTopic, response);
                                }
                            }
                        }
                    } catch( std::exception& exc){

                    }
                } 
            }); 
        }
    }

MqttCallbacks::~MqttCallbacks(){
    {
        // Lock the queue to update the stop flag safely 
        std::unique_lock<std::mutex> lock(queueMutex); 
        stopExecution = true; 
    }

    // Notify all threads 
    conditionVariable.notify_all(); 

    // Joining all worker threads to ensure they have 
    // completed their tasks 
    for (auto& thread : rcvHandlersThreads) { 
        thread.join(); 
    } 
}

void MqttCallbacks::onConnect(std::function<void()> onConnectCallback){
    _onConnectCallback = onConnectCallback;
}

void MqttCallbacks::onDisconnect(std::function<void()> onDisconnectCallback){
    _onDisconnectCallback = onDisconnectCallback;
}

void MqttCallbacks::onPublishResult(std::function<void(PublishResult result, int messageId,  mqtt::const_message_ptr msg)> onPublishResultCallback){
    _onPublishResultCallback = onPublishResultCallback;
}

void MqttCallbacks::on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler){
    for(auto messageHandler : messageHandlers){
        if(std::get<0>(messageHandler) == topicFilter){
            return;
        }
    }
    messageHandlers.emplace_back(topicFilter, messageHandler);
    if(_mqttClient.is_connected()){
        _mqttClient.subscribe(topicFilter, 0, nullptr, *this);
    }
}