#pragma once

#include <cstdint>
#include <cstddef>
#include "mbed.h"
#include "color.h"

namespace iotea {
    namespace colorsensor {

        using ColorRGB = Color<uint8_t>;
        using ColorData = Color<int32_t>;

        class ColorSensor {
        public:
            enum class Frequency {
                DOWN = 0,
                SCALE_2 = 1,
                SCALE_20 = 2,
                SCALE_100 = 3
            };

            enum class Filter {
                RED = 0,
                BLUE = 1,
                CLEAR = 2,
                GREEN = 3
            };

            ColorSensor(
                    PinName s0, PinName s1, PinName s2, PinName s3, PinName out) noexcept;

            /**
             * Sets frequency prescaler.
             * Defaults to Frequency::SCALE_100
             **/
            void setFrequency(Frequency frequency) noexcept;

            /**
             * Sets photodide filter pins.
             **/
            void setFilter(Filter filter) noexcept;

            /**
             * Sets calibration data for black color.
             **/
            void calibrateBlack(ColorData colorData) noexcept;

            /**
             * Sets calibration data for white color.
             **/
            void calibrateWhite(ColorData colorData) noexcept;

            /**
             * Read sensor data in standard RGB form.
             * @returns RGB color.
             **/
            ColorRGB getRGB() const noexcept;

            /**
             * Reads sensor data in raw form.
             * @returns color raw data.
             **/
            ColorData getData() const noexcept;

        protected:
            /// Pins used for frequency scaler
            DigitalOut s0, s1;
            /// Pins used for photodide filter
            DigitalOut s2, s3;
            /// Output pin
            InterruptIn out;
            /// Sensor raw data in current read
            ColorData colorData{0, 0, 0};
            /// Sensor calibration data
            ColorData calibrationBlack{6000L, 6000L, 6000L};
            ColorData calibrationWhite{55000L, 55000L, 55000L};

        private:
            /**
             * Reads counter value and saves it to the current state.
             **/
            void readCounter() noexcept;

            /// Current state of the counter
            size_t state;
            Ticker ticker;
            int32_t counter;

            void convertRGB(ColorRGB *colorRGB) const noexcept;
        };
    }
}