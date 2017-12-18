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

            Serial serial_;
            radio::Radio radio_;

            time_t lastPingMessage_;

            std::list<std::string> pendingForwardMessages_;
            std::map<NodeName, std::list<std::string>> pendingRadioMessages_;
        };
    }
}
