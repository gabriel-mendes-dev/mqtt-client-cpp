#pragma once

#include <mqtt/async_client.h>

class MqttCallbacks: public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
    private:
        // for reconnection, subscription, etc
        mqtt::async_client& _mqttClient; 
        mqtt::connect_options& _connOpts;

        // class user callbacks
        std::function<void()> _onConnectCallback = nullptr;
        std::vector<std::tuple<std::string, std::function<std::string(std::string)>>> messageHandlers;

        void reconnect();

        // (Re)connection success in callback class
        void connected(const std::string& cause) override;
        
        // callback for when the connection is lost.
        // This will initiate the attempt to manually reconnect.
        void connection_lost(const std::string& cause) override;

        // callback for when a message arrives.
        void message_arrived(mqtt::const_message_ptr msg) override;

        // iaction_listener action types
        std::string getTokenTypeStr(mqtt::token::Type type);

        // (Re)connection and subscription failure in iaction_listener class
        void on_failure(const mqtt::token& tok) override;

        // (Re)connection and subscription success in iaction_listener class
        void on_success(const mqtt::token& tok) override;

        // Callback for message delivery complete - not used yet
        void delivery_complete(mqtt::delivery_token_ptr token) override {}

    public:
        MqttCallbacks(mqtt::async_client& mqttClient, mqtt::connect_options& connOpts);

        // for class user to add callback for when client is connected
        void onConnect(std::function<void()> onConnectCallback);

        // for class user to add callbacks for messages received in specific topics
        void on(std::string topic, std::function<std::string(std::string)> messageHandler);

};