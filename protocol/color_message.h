#pragma once

#include "message.h"
#include "../lib/colorsensor/colorsensor.h"

namespace iotea {
    namespace protocol {
        const char ColorMessageType = 100;
        const size_t ColorMessageSize = sizeof(MessageType) + sizeof(uint8_t) * 3;

        class ColorMessage : public Message {
        public:
            ColorMessage(colorsensor::ColorRGB color);
            bytes_t get_bytes() override;

        private:
            colorsensor::ColorRGB color;
        };
    }
}