#include "settings.h"

namespace iotea {
    namespace radio {
        const uint64_t DATA_RATE        = NRF24L01P_DATARATE_250_KBPS;
        const uint64_t POWER            = NRF24L01P_TX_PWR_ZERO_DB;
        const uint64_t CHANNEL          = 2;
        const uint32_t TRANSFER_SIZE    = 32;

        std::map<NodeName, NodeSettings> SETTINGS({
                {NodeName::GATEWAY_OOLONG,  NodeSettings{0xABCDEF01}},
                {NodeName::NODE_EARLGREY,   NodeSettings{0xABCDEF02}},
                {NodeName::NODE_PUERH,      NodeSettings{0xABCDEF03}},
                {NodeName::NODE_YERBA,      NodeSettings{0xABCDEF04}},
                });
    }
}
