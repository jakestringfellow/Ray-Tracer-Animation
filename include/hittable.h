#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include "main.h"
#include "aabb.h"
#include <optional>


class material;

// Stores data related to ray-object hits
struct hit_record {
    point3 p;                       // point at which a ray hits an object
    vec3 normal;                    // Normal vector at hit point
    shared_ptr<material> mat_ptr;   // Shared ptr to hit object material
    double t;                       // ray parameter at which the hit occurred
    double u;                       // text coord u
    double v;                       // text coord v
    bool front_face;                // indicates if the hit occurred on the front face

    // Set the front face and normal based on the direction of the ray and outward normal
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

// Abstract base class representing objects that can be intersected by rays
class hittable {
    public: 
        virtual ~hittable() = default;

        //virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;

        // Determines if a ray hits the object within a specified interval
        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

        // Returns the bounding box for the object
        virtual aabb bounding_box() const = 0;

        // returns the center point of the object
        virtual point3 get_center() const {
            return point3(0, 0, 0);  // Default value for generic hittable
        }



};

// Type of htitable object that moves another object in space
class translate : public hittable {
  public:
    translate(shared_ptr<hittable> p, const vec3& displacement)
      : object(p), offset(displacement)
    {
        // Applies the offset translation to the objects bb
        bbox = object->bounding_box() + offset;
    }

    // Adjusts the ray's origin by the negative offset, checks if the offset ray hits the object
    // Adjusts the hit point by the positive offset
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Move the ray backwards by the offset
        ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine where (if any) an intersection occurs along the offset ray
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    // Return the translated bounding box
    aabb bounding_box() const override { return bbox; }

  private:
    shared_ptr<hittable> object;    // Object being translated
    vec3 offset;                    // Vector indicating how much the object is moving
    aabb bbox;                      // translated bounding box
};


// Type of hittable object that rotates another 'hittable' object around the Y-axis
class rotate_y : public hittable {
  public:

    rotate_y(shared_ptr<hittable> p, double angle) : object(p) {

        // Convert the degrees to radians
        auto radians = degrees_to_radians(angle);

        // Calculate the sine and cosine of the angle
        sin_theta = sin(radians);       
        cos_theta = cos(radians);

        bbox = object->bounding_box();

        point3 min( infinity,  infinity,  infinity);
        point3 max(-infinity, -infinity, -infinity);

        // Iterate over the 8 corners of the original bounding box.
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {

                    // For each corner, these lines calculate the x, y, and z coordinates by
                    // linearly interpolating between the min and max values based on the loop indices
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    // The rotation transformation is applied to the x and z coordinates
                    // This results in new coordinates newx and newz.
                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    // Tester vector created using new x and z coords and the original y coord
                    vec3 tester(newx, y, newz);

                    // For each coordinate axis, the minimum and maximum values are updated based on the 
                    // tester vector
                    // After all the corners have been processed, min and max will represent the smallest and largest x, 
                    // y, and z values of the rotated bounding box, respectively.
                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], tester[c]);
                        max[c] = fmax(max[c], tester[c]);
                    }
                }
            }
        }

        // A new bb is constructed for the rotated object using the computed min and max points
        bbox = aabb(min, max);
    }

    // Determines if a ray 'r' intersects with the rotated object within a specific interval ray_t
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        // Change the ray from world space to object space
        auto origin = r.origin();
        auto direction = r.direction();

        // Apply a y-axis rotation transformation to the ray's origin
        origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
        origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

        // Rotation transformation applied to the ray's direction
        direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
        direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

        ray rotated_r(origin, direction, r.time());

        // Determine where (if any) an intersection occurs in object space
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Change the intersection point from object space to world space
        auto p = rec.p;
        p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
        p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

        // Change the normal from object space to world space
        auto normal = rec.normal;
        normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
        normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

        // Update the hit record
        rec.p = p;
        rec.normal = normal;

        return true;
    }

    // Return the bounding box
    aabb bounding_box() const override { return bbox; }



  private:
    shared_ptr<hittable> object; // Original unrotated object
    double sin_theta;
    double cos_theta;
    aabb bbox;
};

#endif