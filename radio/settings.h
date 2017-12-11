#pragma once

#include "common/name.h"

#include "nRF24L01P.h"

#include <map>

namespace iotea {
    namespace radio {
        /**
         * Some global radio settings.
         */
        extern const uint64_t DATA_RATE;
        extern const uint64_t POWER;
        extern const uint64_t CHANNEL;
        extern const uint32_t TRANSFER_SIZE;

        /**
         * Node specific radio settings.
         */
        struct NodeSettings {
            uint64_t address;
        };

        /**
         * Radio configurations.
         */
        extern std::map<NodeName, NodeSettings> SETTINGS;
    }
}
