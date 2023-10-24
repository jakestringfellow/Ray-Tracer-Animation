#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "main.h"

#include <memory>
#include <vector>
#include "aabb.h"

using std::shared_ptr;
using std::make_shared;

// Provides functionality to maintain a list of hittable objects
// Each object in the list will be tested for ray intersections
//  The class ensures that the closest hit is recorded.
class hittable_list : public hittable {
    public: 
        // Hittable objects
        std::vector<shared_ptr<hittable>> objects;

        // Default empty list
        hittable_list() {}

        // List initialized with first object
        hittable_list(shared_ptr<hittable> object) { add(object); }

        // Empties the list
        void clear() { objects.clear(); }

        // Adds a hittable to the list and updates bb
        void add(shared_ptr<hittable> object) { 
            objects.push_back(object);
            bbox = aabb(bbox, object->bounding_box());
        }

        // Removes a specified object from the list
        void remove(shared_ptr<hittable> object) {
            // Find the target's index
            auto target = std::find(objects.begin(), objects.end(), object);
            // Erase that element
            objects.erase(target);
        }

        // Checks for intersection between a ray and all objects in the list
        bool hit(
            const ray& r, interval ray_t, hit_record& rec) const override {
                // Local variables to store 
                hit_record temp_rec;                // the temporary hit record
                auto hit_anything = false;          // whether a hit occurred
                auto closest_so_far = ray_t.max;    // closest hit distance

                // Checks each object in the list for a hit
                for (const auto& object : objects) {
                    if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                        hit_anything = true;
                        closest_so_far = temp_rec.t;    // update closest hit distance
                        rec = temp_rec;                 // and hit record
                    }
                }

                return hit_anything;

            }


        // Returns the bounding box of the entire list
        aabb bounding_box() const override { return bbox; }

    
    private:
        // Combined bounding box of all list objects
        aabb bbox;
};



#endif