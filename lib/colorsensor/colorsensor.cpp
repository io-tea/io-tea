#include "colorsensor.h"
#include <unordered_map>

using namespace iotea::colorsensor;

namespace {
    const std::unordered_map<size_t, ColorSensor::Filter> stateFilter{
            {0u, ColorSensor::Filter::RED},
            {1u, ColorSensor::Filter::GREEN},
            {2u, ColorSensor::Filter::BLUE}
    };

    template<typename T>
    T clamp(T value, T low, T high) {
        if (value < low) {
            return low;
        } else if (value > high) {
            return high;
        } else {
            return value;
        }
    }
}

ColorSensor::ColorSensor(
        PinName s0, PinName s1, PinName s2, PinName s3, PinName out) noexcept
        : s0{s0}, s1{s1}, s2{s2}, s3{s3}, out{out} {
    // Set defaults
    setFrequency(ColorSensor::Frequency::SCALE_100);
    // Start measuring
    state = 0;
    setFilter(stateFilter.at(state));
    this->out.mode(PullUp);
    this->out.rise([&]{ counter++; });
    ticker.attach([&]{ readCounter(); }, .01);
}

void ColorSensor::setFrequency(ColorSensor::Frequency frequency) noexcept {
    auto f = static_cast<uint8_t>(frequency);
    s0.write(f & 0b10);
    s1.write(f & 0b01);
}

void ColorSensor::setFilter(ColorSensor::Filter filter) noexcept {
    auto f = static_cast<uint8_t>(filter);
    s2.write(f & 0b10);
    s3.write(f & 0b01);
}

void ColorSensor::calibrateBlack(ColorData colorData) noexcept {
    calibrationBlack = colorData;
}

void ColorSensor::calibrateWhite(ColorData colorData) noexcept {
    calibrationWhite = colorData;
}

ColorRGB ColorSensor::getRGB() const noexcept {
    ColorRGB colorRGB(0, 0, 0);
    convertRGB(&colorRGB);
    return colorRGB;
}

ColorData ColorSensor::getData() const {
    return colorData;
}

void ColorSensor::readCounter() noexcept {
    switch (state) {
        case 0:
            colorData.red = counter;
            setFilter(Filter::GREEN);
            break;
        case 1:
            colorData.green = counter;
            setFilter(Filter::BLUE);
            break;
        case 2:
            colorData.blue = counter;
            setFilter(Filter::RED);
            break;
    }
    state = (state + 1) % 3;
    counter = 0;
}

void ColorSensor::convertRGB(ColorRGB *colorRGB) const noexcept {
    int32_t value;

    value = (colorData.red - calibrationBlack.red) * 256;
    value /= calibrationWhite.red - calibrationBlack.red;
    colorRGB->red = static_cast<uint8_t>(clamp(value, 0L, 255L));

    value = (colorData.green - calibrationBlack.green) * 256;
    value /= calibrationWhite.green - calibrationBlack.green;
    colorRGB->green = static_cast<uint8_t>(clamp(value, 0L, 255L));

    value = (colorData.blue - calibrationBlack.blue) * 256;
    value /= calibrationWhite.blue - calibrationBlack.blue;
    colorRGB->blue = static_cast<uint8_t>(clamp(value, 0L, 255L));
}