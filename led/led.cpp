#include "led.h"

namespace iotea {
    namespace led {
        OnBoardLed::OnBoardLed()
            : lastBlinkTime_(0) {}

        void OnBoardLed::configure() {
            led_ = std::make_unique<DigitalOut>(LED1);
        }

        void OnBoardLed::tick() {
            time_t now = time(nullptr);
            if (now > lastBlinkTime_ + 2) {
                *led_ = !*led_;
                lastBlinkTime_ = now;
            }
        }

        void OnBoardLed::handleMessage(const protocol::Message& message) {
            if (message.type == protocol::MessageType::BLINK) {
                *led_ = !*led_;
                lastBlinkTime_ = time(nullptr);
            }
        }
    }
}
