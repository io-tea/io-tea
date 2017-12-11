#include "message.h"

using namespace iotea::protocol;

Message::Message(MessageType type, uint8_t data) type_(type), data_(data) {}
Message::Message(bytes_t bytes) type(bytes[0]), data_(bytes[1]) {}

bytes_t Message::get_bytes() {
    bytes_t bytes;
    bytes[0] = type_;
    bytes[1] = data_;
}
