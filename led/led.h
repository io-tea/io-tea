# pragma once

#include "sensor/sensor.h"

#include "mbed.h"

namespace iotea {
    namespace led {
        /**
         * Led present on every board. Used for status signalization.
         */
        class OnBoardLed : public sensor::Sensor {
        public:
            OnBoardLed();

            void configure() override;
            void tick() override;
            void handleMessage(const protocol::Message& message) override;
            std::list<std::unique_ptr<protocol::Message>> getMessages() override;

        private:
            time_t lastBlinkTime_;
            std::unique_ptr<DigitalOut> led_;
        };
    }
}
