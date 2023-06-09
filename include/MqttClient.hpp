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

        mqtt::create_options _createOptions;
        mqtt::async_client _pahoMqttClient;
        mqtt::connect_options _connectOptions;
        mqtt::ssl_options _sslOptions;
        MqttCallbacks _callbacks;


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
        void publish(std::string topic, std::string payload);
        void on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler);
        void onConnect(std::function<void()> onConnectCallback);
};