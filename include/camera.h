#ifndef CAMERA_H
#define CAMERA_H

#include "main.h"
#include "material.h"

// Camera class responsible for generating rays cast into the scene and determines color returned by rays
class camera {
    public: 
        double aspect_ratio      = 1.0;  // Ratio of image width over height
        int    image_width       = 100;  // Rendered image width in pixels
        int    samples_per_pixel = 10;   // Count of random samples for each pixel (anti-aliasing)
        int    max_depth         = 10;   // Maximum number of ray bounces into scene (ray bounce recursion depth)
        color  background;               // Scene background color       


    public: 
        // Camera constructor
        camera(
            point3 lookfrom, // Point camera is positioned
            point3 lookat,   // Point camera is directed towards
            vec3 vup,        // Identifies the up direction
            double vfov,     // vertical field-of-view in degrees

            // Depth of Field 
            double aspect_ratio,
            double aperture,    // Simulating the adjustable opening in a camera lens (for DoF)
            double focus_dist   // The distance from camera at which objects become out of focus
        ) {

            // Calculating the height and width of the viewport based on the vertical field of view and aspect ratio.
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Setting the camera's origin to the provided lookfrom position
            origin = lookfrom;

            // Depth of Field 
            // Setting up the viewport's dimensions in world space and computing the lower left corner of the viewport
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal/2 - vertical/2 - focus_dist*w;

            // Setting the lens radius to create a depth-of-field effect.
            lens_radius = aperture / 2;
           
        }

        // Creates and returns a ray that originates from the camera and passes through the image plane 
        // at position s, t
        ray get_ray(double s, double t) const {

            // Depth of field effect by using a random offset for the ray's origin.
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            auto ray_time = random_double();

            return ray(
                origin + offset, 
                lower_left_corner + s*horizontal + t*vertical - origin - offset,
                ray_time
            );
        }

        // Given a ray and a list of hittable objects, calculates the color that the ray should return
        // after interacting with the objects in the list.
        // Implements the core ray-color computation logic of ray tracing, considering ray bounces, material emissions, and scatters.
        color ray_color(const ray& r, const hittable& world, int depth) const {
            hit_record rec;

            // If we've exceeded the ray bounce limit, no more light is gathered.
            if (depth <= 0)
                return color(0,0,0);

            // If the ray hits nothing, return the background color.
            if (!world.hit(r, interval(0.001, infinity), rec))
                return background;

            // Check if the ray is scattered by the material of the hit object
            // If not, it returns the emission color
            ray scattered;
            color attenuation;
            color color_from_emission = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

            if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
                return color_from_emission;


            // If the ray does scatter, it calculates the color resulting from that scattering, 
            // combining the scattered color with any emitted color.
            color color_from_scatter = attenuation * ray_color(scattered, world, depth-1);

            return color_from_emission + color_from_scatter;
        }

    private:
        point3 origin;              // Camera position
        point3 lower_left_corner;   // Bottom left corner of cams view (image plane)
        vec3 horizontal;            // Span the image plane for end point
        vec3 vertical;
        // Depth of Field Requirement
        vec3 u, v, w;               // Basis vectors for camera coordinate system
        double lens_radius;         // For DoF effect, half the camera's aperture
};

#endif