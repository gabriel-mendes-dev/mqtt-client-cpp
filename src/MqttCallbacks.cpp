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
        //exit(1);
    }
}

void MqttCallbacks::on_failure(const mqtt::token& tok) {
    //std::cout << "Action failed for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
    if(tok.get_type() == mqtt::token::CONNECT){
        reconnect();
    }
}

void MqttCallbacks::on_success(const mqtt::token& tok) {
    //std::cout << "Action succeeded for MQTT client " << _mqttClient.get_client_id() << ": " << getTokenTypeStr(tok.get_type()) << std::endl;
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

void MqttCallbacks::message_arrived(mqtt::const_message_ptr msg){
    //std::cout << "Message arrived from broker" << std::endl;
    //std::cout << "topic: '" << msg->get_topic() << "'" << std::endl;
    //std::cout << "payload: '" << msg->to_string() << std::endl;
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

MqttCallbacks::MqttCallbacks(mqtt::async_client& mqttClient, mqtt::connect_options& connOpts)
    : _mqttClient(mqttClient), _connOpts(connOpts) {}

void MqttCallbacks::onConnect(std::function<void()> onConnectCallback){
    _onConnectCallback = onConnectCallback;
}

void MqttCallbacks::onDisconnect(std::function<void()> onDisconnectCallback){
    _onDisconnectCallback = onDisconnectCallback;
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