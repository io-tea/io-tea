#include "gateway.h"

#include "protocol/message.h"

namespace iotea {
    namespace gateway {

        const std::vector<NodeName> Gateway::NODES = {
            NodeName::NODE_EARLGREY, NodeName::NODE_PUERH, NodeName::NODE_YERBA};

        const size_t Gateway::PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT = 16;
        const size_t Gateway::PENDING_FORWARD_MESSAGES_QUEUE_SIZE_LIMIT = 32;

        Gateway::Gateway()
            : serial_(USBTX, USBRX)
            , radio_(iotea::NodeName::GATEWAY_OOLONG)
            , lastPingMessage_(0) {}

        void Gateway::setupSerial() {
            serial_.baud(115200);
            serial_.printf("gateway: serial configured\r\n");
        }

        void Gateway::setupRadio() {
            radio_.configure();
        }

        void Gateway::sendNodeMessages(NodeName node) {
            for (auto it = pendingRadioMessages_[node].begin(); it != pendingRadioMessages_[node].end(); ++it) {
                if (radio_.sendData(node, *it)) {
                    serial_.printf("gateway: sent radio message to node %d\r\n", node);
                    it = pendingRadioMessages_[node].erase(it);
                }
                else {
                    serial_.printf("gateway: failed to send radio message to node %d\r\n", node);
                    ++it;
                }
            }
        }

        void Gateway::run() {
            // Set RTC to 2017-01-01
            set_time(0);

            setupSerial();
            setupRadio();

            while (true) {
                while (radio_.isDataAvailible()) {
                    pendingForwardMessages_.emplace_back(radio_.receiveData());
                }

                // TODO:
                // if (wifi_.isDataAvailible()) {
                //     auto message = wifi_.receiveData();
                //     pendingRadioMessages_[message.getTargetNode()].push_back(message);
                // }

                for (const auto node: NODES) {
                    sendNodeMessages(node);
                    while (pendingRadioMessages_[node].size() > PENDING_RADIO_MESSAGES_QUEUE_SIZE_LIMIT)
                        pendingRadioMessages_[node].pop_front();
                }

                // TODO:
                // for (auto it = pendingRadioMessages_.begin(); it != pendingRadioMessages_.end();) {
                //     if (forwardMessage(*it))
                //         it = pendingRadioMessages_.erase(it);
                //     else
                //         ++it;
                // }

                while (pendingForwardMessages_.size() > PENDING_FORWARD_MESSAGES_QUEUE_SIZE_LIMIT)
                    pendingForwardMessages_.pop_front();

                wait(0.001);
            }
        }
    }
}
