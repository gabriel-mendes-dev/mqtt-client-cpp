#include "MqttClient.hpp"

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion):
    _createOptions(mqttVersion),
    _pahoMqttClient(hostAddress + ":" + std::to_string(port), clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptions.set_mqtt_version(mqttVersion);
    _connectOptions.set_clean_start(false);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId):
    _pahoMqttClient(hostAddress + ":" + std::to_string(port), clientId, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptions.set_clean_start(false);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId):
    _pahoMqttClient(hostAddress, clientId, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = 1883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptions.set_clean_start(false);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, int mqttVersion):
    _createOptions(mqttVersion),
    _pahoMqttClient(hostAddress, clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = 1883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptions.set_mqtt_version(mqttVersion);
    _connectOptions.set_clean_start(false);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion, sslSettings sslParams):
    _createOptions(mqttVersion),
    _pahoMqttClient(hostAddress + ":" + std::to_string(port), clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptions.set_trust_store(_sslSettings.caCertPath);
    _sslOptions.set_key_store(_sslSettings.clientCertPath);
    _sslOptions.set_private_key(_sslSettings.clientKeyPath);
    _sslOptions.set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptions.set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptions.set_mqtt_version(mqttVersion);
    _connectOptions.set_clean_start(false);
    _connectOptions.set_ssl(_sslOptions);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, sslSettings sslParams):
    _pahoMqttClient(hostAddress + ":" + std::to_string(port), clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptions.set_trust_store(_sslSettings.caCertPath);
    _sslOptions.set_key_store(_sslSettings.clientCertPath);
    _sslOptions.set_private_key(_sslSettings.clientKeyPath);
    _sslOptions.set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptions.set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptions.set_clean_start(false);
    _connectOptions.set_ssl(_sslOptions);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, sslSettings sslParams):
    _pahoMqttClient(hostAddress, clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = 8883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptions.set_trust_store(_sslSettings.caCertPath);
    _sslOptions.set_key_store(_sslSettings.clientCertPath);
    _sslOptions.set_private_key(_sslSettings.clientKeyPath);
    _sslOptions.set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptions.set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptions.set_clean_start(false);
    _connectOptions.set_ssl(_sslOptions);
    _pahoMqttClient.set_callback(_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, int mqttVersion, sslSettings sslParams):
    _createOptions(mqttVersion),
    _pahoMqttClient(hostAddress, clientId, _createOptions, nullptr),
    _callbacks(_pahoMqttClient, _connectOptions)
{
    _hostPort = 8883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptions.set_trust_store(_sslSettings.caCertPath);
    _sslOptions.set_key_store(_sslSettings.clientCertPath);
    _sslOptions.set_private_key(_sslSettings.clientKeyPath);
    _sslOptions.set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptions.set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptions.set_mqtt_version(mqttVersion);
    _connectOptions.set_clean_start(false);
    _connectOptions.set_ssl(_sslOptions);
    _pahoMqttClient.set_callback(_callbacks);
}

void MqttClient::start(){
    std::cout << "Connecting " << _clientId << " to MQTT broker " << _hostAddress << std::endl;
    _pahoMqttClient.connect(_connectOptions, nullptr, _callbacks);
}

void MqttClient::finish(){
    std::cout << "Disconnecting " << _clientId << " from MQTT broker " << _hostAddress << std::endl;
    _pahoMqttClient.disconnect();
    std::cout << _clientId << " finished disconnecting." << std::endl;
}

void MqttClient::publish(std::string topic, std::string payload, int qos, bool retain){
    _pahoMqttClient.publish(topic, payload, qos, retain);
}

void MqttClient::on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler){
    _callbacks.on(topicFilter, messageHandler);
}

void MqttClient::onConnect(std::function<void()> onConnectCallback){
    _callbacks.onConnect(onConnectCallback);
}