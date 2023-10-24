#ifndef POLYGONMESH_H
#define POLYGONMESH_H

#include "hittable.h"
#include "triangle.h"
#include "OBJModel.h"
#include <vector>

class PolygonMesh : public hittable {
public:
    PolygonMesh(OBJModel objModel, std::shared_ptr<material> mat) {
        // Retrieve the vertices and indices from the OBJModel
        std::vector<float> vertices = objModel.getVertices();
        std::vector<uint32_t> indices = objModel.getIndices();

        // Create triangles from the parsed data
        for (size_t i = 0; i < indices.size(); i+= 3) {
            point3 v0(vertices[3 * indices[i]], vertices[3 * indices[i] + 1], vertices[3 * indices[i] + 2]);
            point3 v1(vertices[3 * indices[i+1]], vertices[3 * indices[i+1] + 1], vertices[3 * indices[i+1] + 2]);
            point3 v2(vertices[3 * indices[i+2]], vertices[3 * indices[i+2] + 1], vertices[3 * indices[i+2] + 2]);
            
            triangles.push_back(std::make_shared<triangle>(v0, v1, v2, mat));
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        bool hit_something = false;
        auto closest_so_far = ray_t.max;

        for (const auto& tri : triangles) {
            if (tri -> hit(r, ray_t, rec)) {
                hit_something = true;
                closest_so_far = rec.t;
            }
        }

        return hit_something;
    }

    aabb bounding_box() const override { return bbox; }


private:
    std::vector<std::shared_ptr<triangle>> triangles;
    aabb bbox;
};

#endif