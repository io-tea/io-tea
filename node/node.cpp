#include "node.h"

namespace iotea {
    namespace node {

        const size_t Node::PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT = 32;

        Node::Node(NodeName nodeName)
            : name_(nodeName)
            , serial_(std::make_shared<Serial>(USBTX, USBRX))
            , radio_(name_, serial_) {}

        void Node::addSensor(std::unique_ptr<sensor::Sensor> sensor) {
            sensors_.push_back(std::move(sensor));
        }

        void Node::setupSerial() {
            serial_->baud(115200);
            serial_->printf("node %d: serial configured\r\n", name_);
        }

        void Node::setupRadio() {
            radio_.configure();
        }

        void Node::handleMessage(const protocol::Message& message) {
            serial_->printf("node %d: received message type %d, data %d\r\n", message.type, message.data);
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

                while (radio_.isDataAvailible()) {
                    std::string data = radio_.receiveData();
                    protocol::Message message(data);
                    handleMessage(message);
                    for (auto& sensor: sensors_)
                        sensor->handleMessage(message);
                }

                // Add node's messages (configuration ones, asking for data, etc)
                for (auto& message: getMessages()) {
                    auto bytes = message->get_bytes();
                    pendingRadioMessages_.emplace_back(std::string(bytes.begin(), bytes.end()));
                }

                // Add sensors' messages (data from world, etc)
                for (auto& sensor: sensors_) {
                    for (auto& message: sensor->getMessages()) {
                        pendingRadioMessages_.emplace_back(message->asPUTCoapPacket());
                    }
                }

                for (auto it = pendingRadioMessages_.begin(); it != pendingRadioMessages_.end(); ++it) {
                    if (radio_.sendData(*it))
                        it = pendingRadioMessages_.erase(it);
                    else
                        ++it;
                }

                while (pendingRadioMessages_.size() > PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT)
                    pendingRadioMessages_.pop_front();

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
