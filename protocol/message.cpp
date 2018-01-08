#include "message.h"
#include "cantcoap/cantcoap.h"

using namespace iotea::protocol;

namespace {

// Global message counter
size_t nextMessageID = 0;

// PUT uri
const std::string uriPUT = "val";

}

Message::Message(const std::string& coapPacket) {
    CoapPDU coapPDU((uint8_t*)(coapPacket.c_str()), coapPacket.size());
    type = static_cast<MessageType>(coapPDU.getPayloadPointer()[0]);
    data = coapPDU.getPayloadPointer()[1];
}

Message::Message(MessageType type, uint8_t data)
    : type(type)
    , data(data) {}
Message::Message(bytes_t bytes)
    : type(static_cast<MessageType>(bytes[0]))
    , data(bytes[1]) {}

bytes_t Message::get_bytes() const {
    bytes_t bytes;
    bytes[0] = type;
    bytes[1] = data;
    return bytes;
}

std::string Message::asPUTCoapPacket() const {
    CoapPDU coapPDU;

    coapPDU.setType(CoapPDU::COAP_NON_CONFIRMABLE);
    coapPDU.setCode(CoapPDU::COAP_PUT);
    coapPDU.setMessageID(nextMessageID++);

    // Kinda redundant, but send message type in both, uri and payload
    std::string uri = uriPUT + "/" + std::to_string(type);
    coapPDU.setURI(const_cast<char*>(uri.c_str()), uri.size());

    // Send type and data in payload
    coapPDU.setPayload(&get_bytes()[0], 2);

    return std::string(coapPDU.getPDUPointer(), coapPDU.getPDUPointer() + coapPDU.getPDULength());
}
