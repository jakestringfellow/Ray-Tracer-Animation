#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include <atomic>
#include "main.h"

// Epsilon definition for float comparisons
//static const float kEpsilon = 1e-5;

class triangle : public hittable {
    public: 
        triangle() {}
        triangle(point3 _v0, point3 _v1, point3 _v2, shared_ptr<material> m, bool singleSided = false) 
            : v0(_v0), v1(_v1), v2(_v2), mat_ptr(m), singleSided(singleSided) {
                normal = unit_vector(cross(v1 - v0, v2 - v0));

                // Bounding Volume requirement'
                // Find minimum and maximum x, y, and z coordinates from the triangle's vertices
                float x_min = fmin(v0.x(), fmin(v1.x(), v2.x()));
                float x_max = fmax(v0.x(), fmax(v1.x(), v2.x()));

                float y_min = fmin(v0.y(), fmin(v1.y(), v2.y()));
                float y_max = fmax(v0.y(), fmax(v1.y(), v2.y()));

                float z_min = fmin(v0.z(), fmin(v1.z(), v2.z()));
                float z_max = fmax(v0.z(), fmax(v1.z(), v2.z()));

                // Set the computed bounding box
                bbox = aabb(point3(x_min, y_min, z_min), point3(x_max, y_max, z_max));

                interval x_interval = bbox.x;
                interval y_interval = bbox.y;
                interval z_interval = bbox.z;

                totalNumTris.fetch_add(1);

                // std::cout << "Bounding Box for Triangle:" << std::endl;
                // std::cout << "Min Point: (" << x_interval.min << ", " << y_interval.min << ", " << z_interval.min << ")" << std::endl;
                // std::cout << "Max Point: (" << x_interval.max << ", " << y_interval.max << ", " << z_interval.max << ")" << std::endl;
            };

            

        // Function for ray triangle intersections
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {

        



            //Check if the ray intersects the bounding box. If not, return false.
            if (!bbox.hit(r, ray_t))
              return false;

            extern std::atomic<uint64_t> numRayTrianglesTests;
            numRayTrianglesTests.fetch_add(1);

            // Edge vector computation
            // Edge 1
            vec3 v0v1 = v1 - v0;
            // Edge 2
            vec3 v0v2 = v2 - v0;

            // Compute the cross product fo ray direction and edge 2
            vec3 pvec = cross(r.direction(), v0v2);
            float det = dot(v0v1, pvec);

            // CULLING

            // If the determinant is negative the triangle is backfacing
            // If the determinant is close to 0, the ray misses the triangle
            if (singleSided && det < kEpsilon) return false;
            // ray and triangle are parallel if det is close to 0
            if (det < kEpsilon && det > -kEpsilon) return false;

            // Calculate inverse of the determinant
            float invDet = 1.0 / det;
            vec3 tvec = r.origin() - v0;
            rec.u = dot(tvec, pvec) * invDet;

            // Check if the intersection is outside of the triangle
            if (rec.u < 0.0 || rec.u > 1.0)
                return false;

            // Compute cross product of vector from vertex to ray origin and edge1
            vec3 qvec = cross(tvec, v0v1);
            rec.v = dot(r.direction(), qvec) * invDet;

            // Check if the intersection is outside of the triangle
            if (rec.v < 0 || rec.u + rec.v > 1) return false;

            // Compute where the interesection point is along the ray
            rec.t = dot(v0v2, qvec) * invDet;

            // Check if t is within the valid range:
            if (rec.t <= ray_t.min || rec.t >= ray_t.max || rec.t <= kEpsilon) return false;


            // If t is positive, there was an intersection
            rec.p = r.at(rec.t);
            rec.normal = normal;
            rec.mat_ptr = mat_ptr;

            extern std::atomic<uint64_t> numRayTrianglesIsect;
            numRayTrianglesIsect.fetch_add(1);
            objectIsect.fetch_add(1);

            return true;
        }

        aabb bounding_box() const override { return bbox; }

    
    public:
        point3 v0, v1, v2;
        shared_ptr<material> mat_ptr;
        vec3 normal;
        bool singleSided;
        aabb bbox;

};



    


#endif