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

void MqttCallbacks::reconnect() {
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    try {
        _mqttClient.connect(_connOpts, nullptr, *this);
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << std::endl;
        //exit(1);
    }
}

void MqttCallbacks::on_failure(const mqtt::token& tok) {
    std::cout << "Action failed for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
    if(tok.get_type() == mqtt::token::CONNECT){
        reconnect();
    }
}

void MqttCallbacks::on_success(const mqtt::token& tok) {
    std::cout << "Action succeeded for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
}

void MqttCallbacks::connected(const std::string& cause) {
    std::cout << "Connection success for MQTT client " << _mqttClient.get_client_id() << std::endl;
    for(auto messageHandler : messageHandlers){
        _mqttClient.subscribe(std::get<0>(messageHandler), 0, nullptr, *this);
    }
    if(_onConnectCallback){
        _onConnectCallback();
    }
}

void MqttCallbacks::connection_lost(const std::string& cause){
    std::cout << "Connection lost for MQTT client" << this->_mqttClient.get_client_id() << std::endl;
    if (!cause.empty()){
        std::cout << "Cause: " << cause << std::endl;
    }
    std::cout << "Reconnecting..." << std::endl;
    reconnect();
}

void MqttCallbacks::message_arrived(mqtt::const_message_ptr msg){
    std::cout << "Message arrived from server" << std::endl;
    std::cout << "topic: '" << msg->get_topic() << "'" << std::endl;
    std::cout << "payload: '" << msg->to_string() << std::endl;
    for(auto messageHandler : messageHandlers){
        if(std::get<0>(messageHandler) == msg->get_topic()){
            std::string response = std::get<1>(messageHandler)(msg->to_string());
            mqtt::properties msgProps = msg->get_properties();
            if(msgProps.contains(mqtt::property::code::RESPONSE_TOPIC)){
                const mqtt::property& responseTopicProp = msgProps.get(mqtt::property::code::RESPONSE_TOPIC);
                const std::string& responseTopic = mqtt::get<std::string>(responseTopicProp);
                _mqttClient.publish(responseTopic, response);
            }
        }
    }
}

MqttCallbacks::MqttCallbacks(mqtt::async_client& mqttClient, mqtt::connect_options& connOpts)
    : _mqttClient(mqttClient), _connOpts(connOpts) {}

void MqttCallbacks::onConnect(std::function<void()> onConnectCallback){
    _onConnectCallback = onConnectCallback;
}

void MqttCallbacks::on(std::string topic, std::function<std::string(std::string)> messageHandler){
    for(auto messageHandler : messageHandlers){
        if(std::get<0>(messageHandler) == topic){
            return;
        }
    }
    messageHandlers.emplace_back(topic, messageHandler);
    if(_mqttClient.is_connected()){
        _mqttClient.subscribe(topic, 0, nullptr, *this);
    }
}