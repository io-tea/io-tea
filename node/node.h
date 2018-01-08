#pragma once

#include "sensor/sensor.h"
#include "radio/radio.h"

#include "mbed.h"

#include <cinttypes>
#include <list>
#include <memory>

namespace iotea {
    namespace node {
        class Node {
        public:
            Node(NodeName nodeName);

            void addSensor(std::unique_ptr<sensor::Sensor> sensor);

            void run();

        protected:
            static const size_t PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT;

            void setupSerial();
            void setupRadio();

            void handleMessage(const protocol::Message& message);
            std::list<protocol::Message> getMessages();

            NodeName name_;

            std::shared_ptr<Serial> serial_;
            radio::Radio radio_;

            std::list<std::unique_ptr<sensor::Sensor>> sensors_;
            std::list<std::string> pendingRadioMessages_;
        };
    }
}
