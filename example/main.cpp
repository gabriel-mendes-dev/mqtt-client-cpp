#include "MqttClient.hpp"

int main(int argc, char *argv[]){
    MqttClient mqttClient("localhost", 1883, "me");
    MqttClient otherMqttClient("localhost", 1883, "me-too");

    mqttClient.onConnect([](){
        std::cout << "Yes!!! The first client is connected to the broker and this is the callback!" << std::endl;
    });

    otherMqttClient.onConnect([](){
        std::cout << "Me-too is connected!" << std::endl;
    });

    mqttClient.on("test", [] (std::string payload) -> std::string {
        std::cout << "Message received: " << payload << std::endl;
        return "This is a response in case mqttv5 publisher asks for it";
    });

    otherMqttClient.on("test2", [] (std::string payload) -> std::string {
        std::cout << "Message received: " << payload << std::endl;
        return "This is a response in case mqttv5 publisher asks for it";
    });

    mqttClient.start();
    otherMqttClient.start();

    int c;
    do {
        c = std::tolower(std::cin.get());
        if(c == 'p'){
            mqttClient.publish("test", "This was published by the first client");
        } else if(c == 's'){
            // subscribe the other client to the first topic
            mqttClient.on("test", [] (std::string payload) -> std::string {
                std::cout << "The other one: message received - " << payload << std::endl;
                return "This is a response in case mqttv5 publisher asks for it";
            });
        }
        
    } while(c != 'q');

    // Disconnect
    mqttClient.finish();
    otherMqttClient.finish();
    return EXIT_SUCCESS;
}