#include "node.h"

namespace iotea {
    namespace node {

        Node::Node(NodeName nodeName)
            : name_(nodeName)
            , serial_(USBTX, USBRX)
            , radio_(name_) {}

        void Node::addSensor(std::unique_ptr<sensor::Sensor> sensor) {
            sensors_.push_back(std::move(sensor));
        }

        void Node::setupSerial() {
            serial_.baud(115200);
            serial_.printf("node %d: serial configured\r\n", name_);
        }

        void Node::setupRadio() {
            radio_.configure();
        }

        void Node::handleMessage(const protocol::Message& message) {
            serial_.printf("node %d: received message type %d, data %d\r\n", message.type, message.data);
            /*
            if (message.type == CONFIGURE_TIME)
                configureTime(dynamic_cast<protocol::TimeMessage>(message));
            ...
            */
        }

        std::list<std::unique_ptr<protocol::Message>> Node::getMessages() {
            std::list<std::unique_ptr<protocol::Message>> messages;
            /*
            // On first loop of activity ask gateway if there are any pending
            // messages.
            if (loopsExecuted == 0)
                messages.emplace_back(std::make_unique<protocol::WakeUpMessage>(nodeName));
            */
            return messages;
        }

        void Node::run() {
            // Set RTC to 2017-01-01
            set_time(0);

            setupSerial();
            setupRadio();

            for (auto& sensor: sensors_)
                sensor->configure();

            while (true) {
                // Perform sensors operations
                for (auto& sensor: sensors_)
                    sensor->tick();

                if (radio_.isDataAvailible()) {
                    std::string data = radio_.receiveData();

                    std::unique_ptr<protocol::Message> message;
                    // TODO: use cantcoap, message = message::unpack(data);
                    message = std::make_unique<protocol::Message>(data[0], data[1]);
                    handleMessage(*message);
                    for (auto& sensor: sensors_)
                        sensor->handleMessage(*message);
                }

                // Add node's messages (configuration ones, asking for data, etc)
                for (auto& message: getMessages()) {
                    auto bytes = message->get_bytes();
                    pendingBinaryMessages_.emplace_back(std::string(bytes.begin(), bytes.end()));
                }

                // Add sensors' messages (data from world, etc)
                for (auto& sensor: sensors_) {
                    for (auto& message: sensor->getMessages()) {
                        auto bytes = message->get_bytes();
                        pendingBinaryMessages_.emplace_back(std::string(bytes.begin(), bytes.end()));
                    }
                }

                for (auto it = pendingBinaryMessages_.begin(); it != pendingBinaryMessages_.end(); ++it) {
                    if (radio_.sendData(*it))
                        it = pendingBinaryMessages_.erase(it);
                    else
                        ++it;
                }

                /*
                if (activeTime > 0.01 && loopsExecuted > 10) {
                    sleep(1.00);
                }
                */

                wait(0.001);
            }
        }
    }
}
