#include "message.h"

using namespace iotea::protocol;

Message::Message(MessageType type, uint8_t data)
    : type(type)
    , data(data) {}
Message::Message(bytes_t bytes)
    : type(static_cast<MessageType>(bytes[0]))
    , data(bytes[1]) {}

bytes_t Message::get_bytes() {
    bytes_t bytes;
    bytes[0] = type;
    bytes[1] = data;
    return bytes;
}
