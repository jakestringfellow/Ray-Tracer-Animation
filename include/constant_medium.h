#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "main.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"

// Represents a volume with constant density
// Can be used for things like fog, smoke, or other media that rays might scatter through rather than 
// just bounce off
// Inherits from the hittable class
class constant_medium : public hittable {
  public:
  	// Constructor takes a hittable object that represents the boundary, a density, and a texture
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
      : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(a))
    {}

	// Constructor that takes a hittable, a density, and a color
    constant_medium(shared_ptr<hittable> b, double d, color c)
      : boundary(b), neg_inv_density(-1/d), phase_function(make_shared<isotropic>(c))
    {}

	// Determines if a ray intersects with the medium
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // Print occasional samples when debugging. To enable, set enableDebug true.
        const bool enableDebug = false;
        const bool debugging = enableDebug && random_double() < 0.00001;

		// Determine entry and exit points of the ray as it intersects the boundary of the volume
        hit_record rec1, rec2;

		// If the ray does not intersect the boundary at all (entry point), return false
        if (!boundary->hit(r, interval::universe, rec1))
            return false;

		// If the ray doesn't have an exit point after the entry point, it also returns false.
        if (!boundary->hit(r, interval(rec1.t+0.0001, infinity), rec2))
            return false;

        if (debugging) std::clog << "\nray_tmin=" << rec1.t << ", ray_tmax=" << rec2.t << '\n';

		// Clamps the hit times to the given interval.
        if (rec1.t < ray_t.min) rec1.t = ray_t.min;
        if (rec2.t > ray_t.max) rec2.t = ray_t.max;

		// If the adjusted entry time is after or at the exit time, there's no real intersection.
        if (rec1.t >= rec2.t)
            return false;

		// Ensures the ray doesn't start behind the camera.
        if (rec1.t < 0)
            rec1.t = 0;

		// Calculate the length of the ray segment inside the boundary and determine a random hit distance
		// based on the density.
        auto ray_length = r.direction().length();
        auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
        auto hit_distance = neg_inv_density * log(random_double());

		// If the calculated hit distance exceeds the distance inside the boundary, 
		// there's no hit within the medium.
        if (hit_distance > distance_inside_boundary)
            return false;

		// Calculate the hit point rec.p and time rec.t inside the medium.
        rec.t = rec1.t + hit_distance / ray_length;
        rec.p = r.at(rec.t);

        if (debugging) {
            std::clog << "hit_distance = " <<  hit_distance << '\n'
                      << "rec.t = " <<  rec.t << '\n'
                      << "rec.p = " <<  rec.p << '\n';
        }

		// Set arbitrary values for the hit normal and front face
		// Assigns the phase function as the material of the hit point.
        rec.normal = vec3(1,0,0);  
        rec.front_face = true;     
        rec.mat_ptr = phase_function;

		// There was a hit within the medium
        return true;
    }

	// Return the bounding box of the boundary object
    aabb bounding_box() const override { return boundary->bounding_box(); }

  private:
	// Shared ptr to the boundary object of the medium
    shared_ptr<hittable> boundary;
	// Store the medium's density's negative inverse
    double neg_inv_density;
	// The material that describes how rays scatter within the medium
    shared_ptr<material> phase_function;
};

#endif