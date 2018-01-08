#pragma once

#include <string>
#include <array>

namespace iotea {
    namespace protocol {
        using bytes_t = std::array<uint8_t, 2>;

        enum MessageType : uint8_t {
            UNKNOWN     = 0,

            // Sensors values
            COLOR       = 1,
            TEMPERATURE = 2,
            LIQUID      = 3,
            HUMIDITY    = 4,

            // Commands
            BLINK       = 5
        };

        struct Message {
            Message(MessageType type, uint8_t data);
            Message(bytes_t bytes);
            Message(const std::string& coapPacket);

            bytes_t get_bytes() const;
            std::string asPUTCoapPacket() const;

            MessageType type;
            uint8_t data;
        };
    }
}
