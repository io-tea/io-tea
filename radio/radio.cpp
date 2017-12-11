#include "radio.h"

namespace iotea {
    namespace radio {
        Radio::Radio(NodeName name)
            : name_(name)
            , isGateway_(name == NodeName::GATEWAY_OOLONG)
            , radio_(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2) {}

        void Radio::configure() {
            radio_.powerDown();
            radio_.powerUp();

            radio_.setAirDataRate(DATA_RATE);
            radio_.setRfOutputPower(POWER);
            radio_.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * CHANNEL);
            radio_.setCrcWidth(NRF24L01P_CRC_8_BIT);
            radio_.enableAutoRetransmit(0x0F, 0x0F);

            // By default send data to gateway.
            if (!isGateway_)
                radio_.setTxAddress(SETTINGS[NodeName::GATEWAY_OOLONG].address, 4);

            // First pipe is for receiving data...
            configurePeer(name_, NRF24L01P_PIPE_P0);

            // ...others are used for acks...
            if (isGateway_) {
                // ...during communication with over nodes for gateway.
                uint64_t pipe = NRF24L01P_PIPE_P1;
                for (const auto& node: SETTINGS) {
                    if (node.first == name_)
                        continue;
                    configurePeer(node.first, pipe++);
                }
            }
            else {
                // ...during communication with gateway for othet nodes.
                configurePeer(NodeName::GATEWAY_OOLONG, NRF24L01P_PIPE_P1);
            }

            radio_.enable();
        }

        void Radio::configurePeer(NodeName peerName, uint32_t pipe) {
            radio_.setRxAddress(SETTINGS[peerName].address, 4, pipe);
            radio_.setTransferSize(TRANSFER_SIZE, pipe);
            radio_.enableAutoAcknowledge(pipe);
        }

        bool Radio::sendData(const std::string& data, uint32_t retriesCount) {
            return sendData(NodeName::GATEWAY_OOLONG, data, retriesCount);
        }

        bool Radio::sendData(NodeName name, const std::string& data, uint32_t retriesCount) {
            // It doesn't make any sense
            if (name == name_)
                return false;

            // Data too long.
            if (data.size() >= TRANSFER_SIZE)
                return false;

            if (isGateway_)
                radio_.setTxAddress(SETTINGS[name].address);

            bool sent = false;
            do {
                int bytesSent = radio_.write(0 /* unused parameter */, (char*)(&data[0]), data.size());
                sent = bytesSent == static_cast<int>(data.size());
                wait((rand() % 1000) / 1e6);
            } while (!sent && retriesCount-- > 0);
            return sent;
        }

        bool Radio::isDataAvailible() {
            return radio_.readable();
        }

        std::string Radio::receiveData() {
            std::string data(TRANSFER_SIZE, '\0');
            int bytesRead = radio_.read(NRF24L01P_PIPE_P0, &data[0], TRANSFER_SIZE);
            if (bytesRead < 0)
                return "";
            data.resize(bytesRead);
            return data;
        }
    }
}
