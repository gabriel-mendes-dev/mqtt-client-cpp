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

/* MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, std::string cert, int mqttVersion){
    _hostPort = port;
    _hostAddress = hostAddress;
    _clientId = clientId;
    _secure = true;
    _cert = cert;
    _connectOptions.set_mqtt_version(mqttVersion);
    _connectOptions.set_clean_start(false);
    _pahoMqttClient.set_callback(*_callbacks);
}

MqttClient::MqttClient(std::string hostAddress, int port, std::string clientId, std::string cert){
    MqttClient(hostAddress, port, _clientId, cert, MQTTVERSION_3_1_1);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, std::string cert){
    MqttClient(hostAddress, 1883, clientId, cert);
}

MqttClient::MqttClient(std::string hostAddress, std::string clientId, std::string cert, int mqttVersion){
    MqttClient(hostAddress, 1883, clientId, cert, MQTTVERSION_3_1_1);
} */

int MqttClient::start(){
    try {
        std::cout << "Connecting " << _clientId << " to MQTT broker " << _hostAddress << std::endl;
        _pahoMqttClient.connect(_connectOptions, nullptr, _callbacks);
    } catch (const mqtt::exception& exc) {
		std::cerr << "ERROR: Unable to connect to MQTT server: '"
			<< _hostAddress << "'" << exc << std::endl;
		return -1;
    }
    return 0;
}

int MqttClient::finish(){
    try {
        std::cout << "Disconnecting " << _clientId << " from MQTT broker " << _hostAddress << std::endl;
        _pahoMqttClient.disconnect()->wait();
        std::cout << _clientId << " finished disconnecting." << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << exc << std::endl;
		return -1;
	}
    return 0;
}

void MqttClient::publish(std::string topic, std::string payload){
    _pahoMqttClient.publish(topic, payload);
}

void MqttClient::on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler){
    _callbacks.on(topicFilter, messageHandler);
}

void MqttClient::onConnect(std::function<void()> onConnectCallback){
    _callbacks.onConnect(onConnectCallback);
}