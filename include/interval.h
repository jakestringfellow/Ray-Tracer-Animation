#ifndef INTERVAL_H
#define INTERVAL_H

#include <limits>
#include "main.h"

class interval {
    public:
        double min, max;

        // Default constructor:
        interval() : min(+infinity), max(-infinity) {} 

        // Construct an interval with a min and max val
        interval(double min_val, double max_val) 
            : min(min_val), max(max_val) {}

        // Construct an interval with two other intervals
        interval(const interval& a, const interval& b) 
            : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

        // Checks if a number is within the interval
        bool contains(double x) const {
            return min <= x && x <= max;
        }

        // Returns the length of the interval
        double size() const {
            return max - min;
        }
        
        // Expands the interval by adding/subtracting a given value equally to its ends
        interval expand(double delta) const {
            auto padding = delta/2;
            return interval(min - padding, max + padding);
        }

        // Checks if a number is contained within the interval (not equal to boundaries)
        bool surrounds(double x) const {
            return min < x && x < max;
        }

        // Clamps a number to lie within the interval, even if its outside set to the nearest
        // boundary
        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max;

            return x;
        }

        static const interval empty, universe;
};

// Initialize empty and universe
const interval interval::empty    = interval(+infinity, -infinity); // an interval with no values
const interval interval::universe = interval(-infinity, +infinity); // an interval that contains all real numbers

// Overload + to add a displacement to both ends
interval operator+(const interval& ival, double displacement) {
    return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval& ival) {
    return ival + displacement;
}

#endif