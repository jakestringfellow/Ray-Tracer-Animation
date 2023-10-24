#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

// Gamma corrects a color value then writes it out to some stream
void write_color(std::ostream &out, color pixel_color, int samples_per_pixel) {

    auto r = pixel_color.x();   // Red
    auto g = pixel_color.y();   // Green
    auto b = pixel_color.z();   // Blue


    // Gamma correction and antialiasing
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif