#ifndef AABB_H
#define AABB_H

#include "main.h"
#include "interval.h"

// Axis Aligned Bounding Box 
class aabb {
    public: 
        // Representing the min/max values for each axis
        interval x, y, z;

        // The default AABB is empty, since intervals are empty by default
        aabb() {} 

        // Interval constructor
        aabb(const interval& ix, const interval& iy, const interval& iz) 
            : x(ix), y(iy), z(iz) { }

        // Point constructor
        aabb(const point3& a, const point3& b) {
            // Treat the two points a and b as extrema for the bounding box, so we don't require a
            // particular minimum/maximum coordinate order.
            x = interval(fmin(a[0],b[0]), fmax(a[0],b[0]));
            y = interval(fmin(a[1],b[1]), fmax(a[1],b[1]));
            z = interval(fmin(a[2],b[2]), fmax(a[2],b[2]));
        }

        // Merges two bounding box to create one large box
        aabb(const aabb& box0, const aabb& box1) {
            x = interval(box0.x, box1.x);
            y = interval(box0.y, box1.y);
            z = interval(box0.z, box1.z);
        }

        // Adds a margin
        // Avoids situations where a quad lying in the xy, yz, or zx plane would be missed due to a width of 0;
        aabb pad() {
            // Return an AABB that has no side narrower than some delta (provides padding)
            double delta = 0.0001;
            interval new_x = (x.size() >= delta) ? x : x.expand(delta);
            interval new_y = (y.size() >= delta) ? y : y.expand(delta);
            interval new_z = (z.size() >= delta) ? z : z.expand(delta);

            return aabb(new_x, new_y, new_z);
        }

        // Returns the interval for an axis
        const interval& axis(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }
       
        // Test for ray intersection
        // Employing the slab method
        bool hit(const ray& r, interval ray_t) const {

            // Compute the parameters at which the ray interesects the x-axis slabs of the bounding box
            float txmin = (x.min - r.origin().x()) / r.direction().x();
            float txmax = (x.max - r.origin().x()) / r.direction().x();

            // Ensure txmin is smaller than txmax
            if (txmin > txmax) std::swap(txmin, txmax);

            // Compute the parameters at which the ray interesects the y-axis slabs of the bounding box
            float tymin = (y.min - r.origin().y()) / r.direction().y();
            float tymax = (y.max - r.origin().y()) / r.direction().y();

            // Ensure tymin is smaller than tymax
            if (tymin > tymax) std::swap(tymin, tymax);

            // The x and y slabs need to overlap for the ray to intersect the box
            if ((txmin > tymax) || (tymin > txmax)) {
                return false;
            }

            // Update the t-values by considering both axes
            // Find the overlapping t-interval between the two slabs
            if (tymin > txmin) txmin = tymin;
            if (tymax < txmax) txmax = tymax;

            // Compute the parameters at which the ray interesects the z-axis slabs of the bounding box
            float tzmin = (z.min - r.origin().z()) / r.direction().z();
            float tzmax = (z.max - r.origin().z()) / r.direction().z();

            // Ensure tzmin is smaller than tzmax
            if (tzmin > tzmax) std::swap(tzmin, tzmax);

            // The current t-interval and the z-axis slab need to overlap for ray-box intersection
            if ((txmin > tzmax) || (tzmin > txmax)) {
                return false;
            }

            // Update the t-values considering all three axes
            // It narrows down the t-interval where the ray intersects the bounding box.
            if (tzmin > txmin) txmin = tzmin;
            if (tzmax < txmax) txmax = tzmax;

            // Update the ray_t interval to reflect the exact t-values where it intersects the box
            ray_t.min = txmin;
            ray_t.max = txmax;

            // Increment the bounding volume intersection counter
            boundingVolumeIsect.fetch_add(1);

            // The ray intersects the bounding box
            return true;
            
        }

        
            
};

// Adds an offset to move a bounding box
aabb operator+(const aabb& bbox, const vec3& offset) {
    return aabb(bbox.x + offset.x(), bbox.y + offset.y(), bbox.z + offset.z());
}

aabb operator+(const vec3& offset, const aabb& bbox) {
    return bbox + offset;
}

#endif
