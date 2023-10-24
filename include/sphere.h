#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"
#include <optional>


class sphere : public hittable {
    public: 
        

        sphere() {}
        // Stationary
        sphere(point3 cen, double r, shared_ptr<material> m) 
            : center1(cen), radius(r), mat_ptr(m), is_moving(false) {
                // Bounding Volume requirement
                auto rvec = vec3(radius, radius, radius);
                bbox = aabb(center1 - rvec, center1 + rvec);
            };

        // Moving
        sphere(point3 cen1, point3 cen2, double r, shared_ptr<material> m)
            : center1(cen1), radius(r), mat_ptr(m), is_moving(true)
            {
                // Bounding Volume requirement
                auto rvec = vec3(radius, radius, radius);
                aabb box1(cen1 - rvec, cen1 + rvec);
                aabb box2(cen2 - rvec, cen2 + rvec);
                bbox = aabb(box1, box2);
                
                center_vec = cen2 - cen1;
            }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

            // Check if the ray intersects the bounding box. If not, return false.
            if (!bbox.hit(r, ray_t))
                return false;

            // checks if the sphere is moving and calculates the current center accordingly.
            point3 center = is_moving ? this->center(r.time()) : center1;

            // calculate whether the ray intersects the sphere
            // determine the coefficients for the quadratic equation derived from the ray-sphere intersection test
            vec3 oc = r.origin() - center;
            auto a = r.direction().length_squared();
            auto half_b = dot(oc, r.direction());
            auto c = oc.length_squared() - radius*radius;

            // If the discriminant of this quadratic equation is negative, it means the ray doesn't intersect the sphere
            auto discriminant = half_b*half_b - a*c;

            if (discriminant < 0) return false;
            
            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (-half_b - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (-half_b + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            // The outward normal at the intersection point is calculated and normalized (by dividing by the sphere's radius)
            // This outward normal is then used to set the face normal
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 outward_normal = (rec.p - center) / radius;
            rec.set_face_normal(r, outward_normal);

            // Rotate the outward_normal using rotation matrices, if there is an offset, it is accounted for
            vec3 rotated_normal = outward_normal;

            // X-axis rotation
            rotated_normal = vec3(
                rotated_normal.x(),
                cos(uv_rotation_offset_x) * rotated_normal.y() - sin(uv_rotation_offset_x) * rotated_normal.z(),
                sin(uv_rotation_offset_x) * rotated_normal.y() + cos(uv_rotation_offset_x) * rotated_normal.z()
            );

            // Y-axis rotation
            rotated_normal = vec3(
                cos(uv_rotation_offset_y) * rotated_normal.x() + sin(uv_rotation_offset_y) * rotated_normal.z(),
                rotated_normal.y(),
                -sin(uv_rotation_offset_y) * rotated_normal.x() + cos(uv_rotation_offset_y) * rotated_normal.z()
            );

            // Z-axis rotation
            rotated_normal = vec3(
                cos(uv_rotation_offset_z) * rotated_normal.x() - sin(uv_rotation_offset_z) * rotated_normal.y(),
                sin(uv_rotation_offset_z) * rotated_normal.x() + cos(uv_rotation_offset_z) * rotated_normal.y(),
                rotated_normal.z()
            );

            // Compute the UVs for the rotated normal
            get_sphere_uv(rotated_normal, rec.u, rec.v);

            // Ensure u and v are in [0, 1] range.
            if (rec.u >= 1.0) rec.u -= 1.0;
            if (rec.u < 0.0) rec.u += 1.0;
            if (rec.v > 1.0) rec.v -= 1.0;
            if (rec.v < 0.0) rec.v += 1.0;

            rec.mat_ptr = mat_ptr;
            objectIsect.fetch_add(1);

            return true;
        }

        aabb bounding_box() const override { return bbox; }

        point3 get_center() const override {
            return center1;  
        }

        void translate(const point3& offset) {
            center1 += offset;
            bbox = bbox + offset;
        }

        void rotate(std::string axis, double degrees) {
            double radians = degrees * M_PI / 180.0; // Convert degrees to radians
            if (axis == "x") {
                uv_rotation_offset_x += radians;
            } else if (axis == "y") {
                uv_rotation_offset_y += radians;
            } else if (axis == "z") {
                uv_rotation_offset_z += radians;
            }
            
        }

    
    private:
        point3 center1;
        double radius;
        shared_ptr<material> mat_ptr;
        bool is_moving;
        vec3 center_vec;
        aabb bbox;
        //double uv_rotation_offset = 0.0;
        double uv_rotation_offset_x = 0.0;
        double uv_rotation_offset_y = 0.0;
        double uv_rotation_offset_z = 0.0;


        point3 center(double time) const {
            // Linearly interpolate from center1 to center2 according to time, where t=0 yields
            // center1 and t=1 yields center2.
            return center1 + time*center_vec;
        }

        static void get_sphere_uv(const point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
};



#endif