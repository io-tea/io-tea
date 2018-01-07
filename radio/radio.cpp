#include "radio.h"

#include <vector>

namespace iotea {
    namespace radio {
        Radio::Radio(NodeName name)
            : name_(name)
            , isGateway_(name == NodeName::GATEWAY_OOLONG)
            , currentPipeWithDataIndex_(0)
            , radio_(PB_15, PB_14, PB_13, PB_12, PB_1, PB_2) {}

        void Radio::configure() {
            radio_.powerDown();
            radio_.powerUp();

            radio_.setAirDataRate(DATA_RATE);
            radio_.setRfOutputPower(POWER);
            radio_.setRfFrequency(NRF24L01P_MIN_RF_FREQUENCY + 4 * CHANNEL);
            radio_.setCrcWidth(NRF24L01P_CRC_8_BIT);
            radio_.enableAutoRetransmit(0x01, 0x01);

            if (isGateway_) {
                auto pipe = NRF24L01P_PIPE_P0;
                for (const auto& node: SETTINGS) {
                    radio_.enableAutoAcknowledge(pipe);
                    radio_.setRxAddress(node.second.address, 5, pipe);
                    radio_.setTransferSize(TRANSFER_SIZE, pipe);
                }
                radio_.setReceiveMode();
            }
            else {
                radio_.enableAutoAcknowledge(NRF24L01P_PIPE_P0);
                radio_.setRxAddress(SETTINGS[name_].address, 4, NRF24L01P_PIPE_P0);
                radio_.setTxAddress(SETTINGS[name_].address, 4);
                radio_.setTransferSize(TRANSFER_SIZE, NRF24L01P_PIPE_P0);
            }

            radio_.enable();
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

            static std::vector<char> txData(TRANSFER_SIZE, '\0');
            std::copy(data.begin(), data.end(), txData.begin());
            std::fill(txData.begin() + data.size(), txData.end(), '\0');

            bool sent = false;
            do {
                int bytesSent = radio_.write(0 /* unused parameter */, &txData[0], TRANSFER_SIZE);
                sent = bytesSent == TRANSFER_SIZE;
                wait((rand() % 1000) / 1e6);
            } while (!sent && retriesCount-- > 0);
            return sent;
        }

        bool Radio::isDataAvailible() {
            if (isGateway_) {
                for (size_t i = 0; i < SETTINGS.size(); ++i) {
                    if (radio_.readable(NRF24L01P_PIPE_P0 + currentPipeWithDataIndex_))
                        return true;
                    currentPipeWithDataIndex_ = (currentPipeWithDataIndex_ + 1) % SETTINGS.size();
                }
            }
            else {
                return radio_.readable(NRF24L01P_PIPE_P0);
            }

            return false;
        }

        std::string Radio::receiveData() {
            std::string data(TRANSFER_SIZE, '\0');
            int bytesRead = radio_.read(NRF24L01P_PIPE_P0 + currentPipeWithDataIndex_, &data[0], TRANSFER_SIZE);
            if (bytesRead < 0)
                return "";
            data.resize(bytesRead);
            return data;
        }
    }
}
