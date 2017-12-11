#include "node.h"

namespace iotea {
    namespace node {

        Node::Node(radio::NodeName radioNodeName)
            : serial_(USBTX, USBRX)
            , radio_(radioNodeName) {}

        void Node::addSensor(std::unique_ptr<sensor::Sensor> sensor) {
            sensors_.push_back(std::move(sensor));
        }

        void Node::setupSerial() {
            serial_.baud(115200);
            serial_.printf("node %d: serial configured\r\n");
        }

        void Node::setupRadio() {
            radio_.configure();
        }

        void Node::handleMessage(const protocol::Message& message) {
            /*
            if (message.type == CONFIGURE_TIME)
                configureTime(dynamic_cast<protocol::TimeMessage>(message));
            ...
            */
        }

        std::list<std::unique_ptr<protocol::Message>> getMessages() {
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
            setupSerial();
            setupRadio();

            for (auto& sensor: sensors_)
                sensor->configure();

            while (true) {
                if (radio_.isDataAvailible()) {
                    std::string data = radio_.receiveData();

                    std::unique_ptr<protocol::Message> message;
                    // TODO: message = message::unpack(data);
                    handleMessage(*message);
                    for (auto& sensor: sensors_)
                        sensor->handleMessage(*message);
                }

                // Add node's messages (configuration ones, asking for data, etc)
                for (auto& message: getMessages())
                    pendingBinaryMessages_.emplace_back(message->get_bytes());

                // Add sensors' messages (data from world, etc)
                for (auto& sensor: sensors_)
                    for (auto& message: sensor->getMessages())
                        pendingBinaryMessages_.emplace_back(message->get_bytes());

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
