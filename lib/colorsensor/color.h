#pragma once

namespace iotea {
    namespace colorsensor {
        template<typename T>
        struct Color {
            T red, green, blue;

            Color(T red, T green, T blue) : red(red), green(green), blue(blue) {}
        };
    }
}