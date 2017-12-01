#include "color_message.h"

using namespace iotea::protocol;

ColorMessage::ColorMessage(colorsensor::ColorRGB color): color(color) {

}

bytes_t ColorMessage::get_bytes() {
    bytes_t bytes{ColorMessageSize};

    uint8_t * data = reinterpret_cast<uint8_t *>(const_cast<char *>(bytes.data()));
    data[0] = ColorMessageType;
    data[1] = color.red;
    data[2] = color.green;
    data[3] = color.blue;

    return bytes;
}