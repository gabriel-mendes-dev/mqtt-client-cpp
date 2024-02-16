#pragma once

#include <mqtt/async_client.h>
#include <thread>

class MqttCallbacks: public virtual mqtt::callback, public virtual mqtt::iaction_listener
{
    public:
        enum PublishResult {
            PUBLISH_SUCCESS = 0,
            PUBLISH_FAILURE = 1
        };

    private:
        // for reconnection, subscription, etc
        mqtt::async_client& _mqttClient; 
        mqtt::connect_options& _connOpts;

        // class user callbacks
        std::function<void()> _onConnectCallback = nullptr;
        std::function<void()> _onDisconnectCallback = nullptr;
        std::function<void(PublishResult result, int messageId, mqtt::const_message_ptr msg)> _onPublishResultCallback = nullptr;
        std::vector<std::tuple<std::string, std::function<std::string(std::string, std::string)>>> messageHandlers;
        
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
        void delivery_complete(mqtt::delivery_token_ptr tok) override;

        bool isMqttTopicIncluded(const std::string& topic, const std::string& filter);

        void messageArrivedHandler(mqtt::const_message_ptr msg);

        // Vector to store worker threads 
        std::vector<std::thread> rcvHandlersThreads; 
    
        // Queue of messages to be handled by workers
        std::queue<mqtt::const_message_ptr> msgQueue; 
    
        // Mutex to synchronize access to shared data 
        std::mutex queueMutex; 
    
        // Condition variable to signal changes in the state of 
        // the messages queue 
        std::condition_variable conditionVariable; 
    
        // Flag to indicate whether the thread pool should stop 
        // or not 
        bool stopExecution = false;

        // Enqueue message for handling by the thread pool 
        void enqueue(mqtt::const_message_ptr msg);

    public:
        MqttCallbacks(mqtt::async_client& mqttClient, mqtt::connect_options& connOpts, int numRcvHandlerThreads = std::thread::hardware_concurrency());
        ~MqttCallbacks();

        // for class user to add callback for when client is connected
        void onConnect(std::function<void()> onConnectCallback);

        // for class user to add callback for when client is disconnected
        void onDisconnect(std::function<void()> onDisconnectCallback);

        void onPublishResult(std::function<void(PublishResult result, int messageId, mqtt::const_message_ptr msg)> onPublishResultCallback);

        // for class user to add callbacks for messages received in specific topics
        void on(std::string topicFilter, std::function<std::string(std::string topic, std::string payload)> messageHandler);

};