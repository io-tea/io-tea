#pragma once

#include <string>

namespace iotea {
    namespace protocol {
        using bytes_t = std::string;
        using MessageType = uint8_t;

        class Message {
        public:
            virtual ~Message() = 0;
            virtual bytes_t get_bytes();
        };
    }
}
