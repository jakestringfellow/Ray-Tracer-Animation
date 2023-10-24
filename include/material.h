#ifndef MATERIAL_H
#define MATERIAL_H

#include "main.h"
#include "hittable.h"
#include "texture.h"

//struct hit_record;

// Base class for all materials
class material {
    public: 
        // Returns a black color for materials that don't emit light
        virtual color emitted(double u, double v, const point3& p) const {
            return color(0,0,0);
        }

        // Decides how a ray scatters off the surface
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const = 0;

};

// Diffuse material
class lambertian : public material {
    public: 
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        // Generates a new ray in a random direction close to the normal of the hit surface
        // This simulates a diffuse reflection.
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            auto scatter_direction = rec.normal + random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = ray(rec.p, scatter_direction, r_in.time());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    private:
        //color albedo;
        shared_ptr<texture> albedo;

};

// Metallic material
class metal : public material {
    public: 
        metal(const color& a, double f) : albedo(make_shared<solid_color>(a)), fuzz(f < 1 ? f : 1) {}
        metal(shared_ptr<texture> a, double f) : albedo(a), fuzz(f<1 ? f : 1){}

        // Reflects the incoming ray in the reflection direction based on the hit surface's normal
        // Adds some fuzziness to simulate a rough metal surface
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
            // scattered = ray(rec.p, reflected);
            //attenuation = albedo;
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        //color albedo;
        shared_ptr<texture> albedo;
        double fuzz;
};

// Material that can refract light (glass, water)
class dielectric : public material {
    public: 
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        // Depending on the angle of incidence, using Schlick's approximation, 
        // either reflects or refracts the incoming ray
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
        ) const override {
            attenuation = color(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            vec3 unit_direction = unit_vector(r_in.direction());

            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else 
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = ray(rec.p, direction, r_in.time());

            return true;
        }

    public:
        double ir; // Index of Refraction

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1 + ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

// Light emitting material
class diffuse_light : public material {
  public:
    diffuse_light(shared_ptr<texture> a) : emit(a) {}
    diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}

    // Does not scatter any light
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        return false;
    }

    // Return sthe emitted lights color based on the texture
    color emitted(double u, double v, const point3& p) const override {
        return emit->value(u, v, p);
    }

  private:
    shared_ptr<texture> emit;
};

// Material that scatters light uniformly in all directions
// Used for fog, smoke, etc.
class isotropic : public material {
  public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    // Generates a ray in a completely random direction, irrespective of surface normals
    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        scattered = ray(rec.p, random_unit_vector(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

  private:
    shared_ptr<texture> albedo;
};

#endif