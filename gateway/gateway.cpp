#include "gateway.h"

#include "protocol/message.h"

namespace iotea {
    namespace gateway {

        const std::vector<NodeName> Gateway::NODES = {
            NodeName::NODE_EARLGREY, NodeName::NODE_PUERH, NodeName::NODE_YERBA};

        Gateway::Gateway()
            : lastPingMessage_(0)
            , serial_(USBTX, USBRX)
            , radio_(iotea::NodeName::GATEWAY_OOLONG) {}

        void Gateway::setupSerial() {
            serial_.baud(115200);
            serial_.printf("gateway: serial configured\r\n");
        }

        void Gateway::setupRadio() {
            radio_.configure();
        }

        void Gateway::sendNodeMessages(NodeName node) {
            for (auto it = pendingRadioMessages_[node].begin(); it != pendingRadioMessages_[node].end(); ++it) {
                if (radio_.sendData(node, *it))
                    it = pendingRadioMessages_[node].erase(it);
                else
                    ++it;
            }
        }

        void Gateway::run() {
            // Set RTC to 2017-01-01
            set_time(0);

            setupSerial();
            setupRadio();

            while (true) {
                if (radio_.isDataAvailible()) {
                    pendingForwardMessages_.emplace_back(radio_.receiveData());
                }

                for (const auto node: NODES)
                    sendNodeMessages(node);

                // TODO:
                // for (auto it = pendingRadioMessages_.begin(); it != pendingRadioMessages_.end();) {
                //     if (forwardMessage(*it))
                //         it = pendingRadioMessages_.erase(it);
                //     else
                //         ++it;
                // }

                wait(0.001);
            }
        }
    }
}
