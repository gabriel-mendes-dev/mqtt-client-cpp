#pragma once

#include <string>
#include <functional>
#include <vector>
#include <tuple>

#include <mqtt/async_client.h>

#include "MqttCallbacks.hpp"

class MqttClient {
    public:
        struct sslSettings {
            std::string caCertPath;
            std::string clientCertPath;
            std::string clientKeyPath;
            std::string clientKeyPassword;
        };

    private:
        std::string _hostAddress;
        int _hostPort;
        std::string _clientId;
        sslSettings _sslSettings;

        std::unique_ptr<mqtt::create_options> _createOptionsPtr;
        std::unique_ptr<mqtt::async_client> _pahoMqttClientPtr;
        std::unique_ptr<mqtt::connect_options> _connectOptionsPtr;
        std::unique_ptr<mqtt::ssl_options> _sslOptionsPtr;
        std::unique_ptr<MqttCallbacks> _callbacksPtr;


    public:
        MqttClient(std::string hostAddress, std::string clientId);
        MqttClient(std::string hostAddress, int port, std::string clientId);
        MqttClient(std::string hostAddress, std::string clientId, int mqttVersion);
        MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion);
        MqttClient(std::string hostAddress, std::string clientId, sslSettings sslParams);
        MqttClient(std::string hostAddress, int port, std::string clientId, sslSettings sslParams);
        MqttClient(std::string hostAddress, std::string clientId, int mqttVersion, sslSettings sslParams);
        MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion, sslSettings sslParams);
        void start();
        void finish();
        int publish(std::string topic, std::string payload, int qos, bool retain);
        void on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler);
        void onConnect(std::function<void()> onConnectCallback);
        void onDisconnect(std::function<void()> onDisconnectCallback);
        void onPublishResult(std::function<void(MqttCallbacks::PublishResult result, int messageId)> onPublishResultCallback);
        bool isConnected();
};