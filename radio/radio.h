#pragma once

#include "settings.h"

#include "nRF24L01P.h"

namespace iotea {
    namespace radio {
        class Radio {
        public:
            Radio(NodeName name);

            /**
             * Setup configuration for this node.
             */
            void configure();

            /**
             * Send data to gateway.
             */
            bool sendData(const std::string& data, uint32_t retriesCount = 16);

            /**
             * Send data to given node. Can be used only in gateway.
             */
            bool sendData(NodeName node, const std::string& data, uint32_t retriesCount = 16);

            /**
             * Wheather it is possible to receive any data.
             */
            bool isDataAvailible();

            /**
             * Read next packet.
             */
            std::string receiveData();

        private:
            NodeName name_;
            bool isGateway_;

            size_t currentPipeWithDataIndex_;

            nRF24L01P radio_;
        };
    }
}
