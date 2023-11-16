#include "MqttClient.hpp"

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion)
{
    _createOptionsPtr = std::make_unique<mqtt::create_options>(mqttVersion);
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress + ":" + std::to_string(port), clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptionsPtr->set_mqtt_version(mqttVersion);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId)
{
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress + ":" + std::to_string(port), clientId, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId)
{
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress, clientId, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = 1883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, int mqttVersion)
{
    _createOptionsPtr = std::make_unique<mqtt::create_options>(mqttVersion);
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress, clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = 1883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _connectOptionsPtr->set_mqtt_version(mqttVersion);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, int mqttVersion, sslSettings sslParams)
{
    _createOptionsPtr = std::make_unique<mqtt::create_options>(mqttVersion);
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress + ":" + std::to_string(port), clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptionsPtr->set_trust_store(_sslSettings.caCertPath);
    _sslOptionsPtr->set_key_store(_sslSettings.clientCertPath);
    _sslOptionsPtr->set_private_key(_sslSettings.clientKeyPath);
    _sslOptionsPtr->set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptionsPtr->set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    // std::cout << "SSL Password: " << _sslSettings.clientKeyPassword << std::endl;
    _sslOptionsPtr->set_error_handler([](const std::string& msg) {
        std::cerr << "SSL Error: " << msg << std::endl;
    });
    _connectOptionsPtr->set_mqtt_version(mqttVersion);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _connectOptionsPtr->set_ssl(*_sslOptionsPtr);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, sslSettings sslParams)
{
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress + ":" + std::to_string(port), clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptionsPtr->set_trust_store(_sslSettings.caCertPath);
    _sslOptionsPtr->set_key_store(_sslSettings.clientCertPath);
    _sslOptionsPtr->set_private_key(_sslSettings.clientKeyPath);
    _sslOptionsPtr->set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptionsPtr->set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _connectOptionsPtr->set_ssl(*_sslOptionsPtr);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, sslSettings sslParams)
{
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress, clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = 8883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptionsPtr->set_trust_store(_sslSettings.caCertPath);
    _sslOptionsPtr->set_key_store(_sslSettings.clientCertPath);
    _sslOptionsPtr->set_private_key(_sslSettings.clientKeyPath);
    _sslOptionsPtr->set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptionsPtr->set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _connectOptionsPtr->set_ssl(*_sslOptionsPtr);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, int mqttVersion, sslSettings sslParams)
{
    _createOptionsPtr = std::make_unique<mqtt::create_options>(mqttVersion);
    _pahoMqttClientPtr = std::make_unique<mqtt::async_client>(hostAddress, clientId, *_createOptionsPtr, nullptr),
    _connectOptionsPtr = std::make_unique<mqtt::connect_options>();
    _sslOptionsPtr = std::make_unique<mqtt::ssl_options>();
    _callbacksPtr = std::make_unique<MqttCallbacks>(*_pahoMqttClientPtr, *_connectOptionsPtr);
    _hostPort = 8883;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _sslSettings = sslParams;
    _sslOptionsPtr->set_trust_store(_sslSettings.caCertPath);
    _sslOptionsPtr->set_key_store(_sslSettings.clientCertPath);
    _sslOptionsPtr->set_private_key(_sslSettings.clientKeyPath);
    _sslOptionsPtr->set_private_key_password(_sslSettings.clientKeyPassword);
    _sslOptionsPtr->set_ssl_version(MQTT_SSL_VERSION_TLS_1_2);
    _connectOptionsPtr->set_mqtt_version(mqttVersion);
    _connectOptionsPtr->set_clean_start(false);
    _connectOptionsPtr->set_keep_alive_interval(std::chrono::seconds(10));
    _connectOptionsPtr->set_automatic_reconnect(true);
    _connectOptionsPtr->set_ssl(*_sslOptionsPtr);
    _pahoMqttClientPtr->set_callback(*_callbacksPtr);
}

void MqttClient::start(){
    //std::cout << "Connecting " << _clientId << " to MQTT broker " << _hostAddress << std::endl;
    _pahoMqttClientPtr->connect(*_connectOptionsPtr, nullptr, *_callbacksPtr);
}

void MqttClient::finish(){
    //std::cout << "Disconnecting " << _clientId << " from MQTT broker " << _hostAddress << std::endl;
    _pahoMqttClientPtr->disconnect();
    //std::cout << _clientId << " finished disconnecting." << std::endl;
}

int MqttClient::publish(std::string topic, std::string payload, int qos, bool retain){
    mqtt::message_ptr msg = mqtt::make_message(topic, payload, qos, retain);
    mqtt::delivery_token_ptr tok = _pahoMqttClientPtr->publish(msg, nullptr, *_callbacksPtr);
    return tok->get_message_id();
}

void MqttClient::on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler){
    _callbacksPtr->on(topicFilter, messageHandler);
}

void MqttClient::onConnect(std::function<void()> onConnectCallback){
    _callbacksPtr->onConnect(onConnectCallback);
}

void MqttClient::onDisconnect(std::function<void()> onDisconnectCallback){
    _callbacksPtr->onDisconnect(onDisconnectCallback);
}

void MqttClient::onPublishResult(std::function<void(MqttCallbacks::PublishResult result, int messageId)> onPublishResultCallback){
    _callbacksPtr->onPublishResult(onPublishResultCallback);
}

bool MqttClient::isConnected(){
    return _pahoMqttClientPtr->is_connected();
}