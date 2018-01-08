#pragma once

#include "radio/radio.h"

#include <memory>
#include <vector>
#include <map>
#include <list>

namespace iotea {
    namespace gateway {
        class Gateway {
        public:
            Gateway();

            void run();

        private:
            void setupSerial();
            void setupRadio();

            void sendNodeMessages(NodeName node);

            static const std::vector<NodeName> NODES;
            static const size_t PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT;
            static const size_t PENDING_FORWARD_MESSAGES_QUEUE_SIZE_LIMIT;

            std::shared_ptr<Serial> serial_;
            radio::Radio radio_;

            time_t lastPingMessage_;

            std::list<std::string> pendingForwardMessages_;
            std::map<NodeName, std::list<std::string>> pendingRadioMessages_;
        };
    }
}
