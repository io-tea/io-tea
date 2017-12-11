#pragma once

#include <string>

namespace iotea {
    namespace protocol {
        using bytes_t = uint8_t[2];

        enum MessageType {
            UNKNOWN,
            COLOR,
            TEMPERATURE,
            LIQUID,
            HUMIDITY
        };

        struct Message {
            Message(MessageType type, uint8_t data);
            Message(bytes_t bytes);
            bytes_t get_bytes();

            MessageType type_;
            uint8_t data_;
        };
    }
}
