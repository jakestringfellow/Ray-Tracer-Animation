#include "../include/main.h"
#include <atomic>
#include <iostream>

#include "../include/color.h"
#include "../include/vec3.h"
#include "../include/ray.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"
#include "../include/camera.h"
#include "../include/material.h"
#include "../include/ppm.h"
#include "../include/triangle.h"
#include "../include/OBJModel.h"
#include "../include/PolygonMesh.h"
#include "../include/texture.h"
#include "../include/quad.h"
#include "../include/constant_medium.h"

// Variables for performance logging
std::atomic<uint64_t> numRayTrianglesTests(0);
std::atomic<uint64_t> numRayTrianglesIsect(0);
std::atomic<uint64_t> boundingVolumeIsect(0);
std::atomic<uint64_t> objectIsect(0);
static uint64_t numPrimaryRays = 0;
std::atomic<uint64_t> totalNumTris(0);


// View requirement
void render_scene(std::ofstream& outFile, const camera& cam, const hittable_list& world, int image_width, int image_height, int samples_per_pixel, int max_depth) {
    outFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0,0,0);
            // Antialiasing requirement
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                numPrimaryRays++;
                pixel_color += cam.ray_color(r, world, max_depth);
            }
            write_color(outFile, pixel_color, samples_per_pixel);
        }
    }
}

//hittable_list day_and_night_setup() {

//     hittable_list world;
//     // Perlin texture
//     //auto pertext = make_shared<noise_texture>(4);
//     // Ground
//     world.add(make_shared<sphere>(point3(69.375,-500,119.375), 500, make_shared<lambertian>(point3(0.5, 0.8, 0.7))));
//     //world.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

//     // poly mesh bunny
//     OBJModel obj = OBJModel("bunny_centered_247_faces.obj");
//     auto obj_material = make_shared<lambertian>(color(0.9, 0.6, 0.4));
//     world.add(make_shared<PolygonMesh>(obj, obj_material));

//     world.add(make_shared<sphere>(point3(69.375,20,119.375), 20, make_shared<metal>(point3(0.5, 0.5, 0.5), 0.5)));



//     return world;
// }

// hittable_list day_and_night_loop(shared_ptr<sphere> sun, hittable_list world, int i) {

//         auto orbit_center = point3(69.375,-500,119.375);


//     // Compute rotation
//         double angle = 15.0 * M_PI / 180.0; //* i;
//         double relative_x = sun->get_center().x() - orbit_center.x();
//         double relative_y = sun->get_center().y() - orbit_center.y();

//         double x_new = relative_x * cos(angle) - relative_y * sin(angle) + orbit_center.x();
//         double y_new = relative_x * sin(angle) + relative_y * cos(angle) + orbit_center.y();
        
//         double x_offset = x_new - sun->get_center().x();
//         double y_offset = y_new - sun->get_center().y();
//         double z_offset = 0; // We're not changing the z-coordinate for the orbit around z-axis.

//         if (std::find(world.objects.begin(), world.objects.end(), sun) != world.objects.end()) {
//             world.remove(sun);
//         }

//         //sphere1 = make_shared<translate>(sphere1, point3(floor_center.x() + x_new, sphere1->get_center().y(), floor_center.z() + z_new));
        
//         sun->translate(point3(x_offset, y_offset, z_offset));
//         //sun->rotate("z", 30);
//         world.add(sun);

    
        // // Compute rotation
        // double angle = 15.0 * M_PI / 180.0; //* i;
        // double relative_x = sphere1->get_center().x() - floor_center.x();
        // double relative_z = sphere1->get_center().z() - floor_center.z();

        // double x_new = relative_x * cos(angle) - relative_z * sin(angle) + floor_center.x();
        // double z_new = relative_x * sin(angle) + relative_z * cos(angle) + floor_center.z();
        
        // double x_offset = x_new - sphere1->get_center().x();
        // double y_offset = 0;  // We're not changing the y-coordinate for the orbit around y-axis.
        // double z_offset = z_new - sphere1->get_center().z();

        // if (std::find(world.objects.begin(), world.objects.end(), sphere1) != world.objects.end()) {
        //     world.remove(sphere1);
        // }

        // //sphere1 = make_shared<translate>(sphere1, point3(floor_center.x() + x_new, sphere1->get_center().y(), floor_center.z() + z_new));
        
        // sphere1->translate(point3(x_offset, y_offset, z_offset));
        // sphere1->rotate("y", 30);
        // world.add(sphere1);

//         return world;

    
// }

hittable_list poke_ball_test(shared_ptr<sphere> pokeball, hittable_list world, int i) {
    // Compute rotation and translation
    // First frame ball is still
    // if (i == 0) {
    //     return world;
    // }
    



    // Second through third frame ball is turning to it's right
    if (i > 0 && i <= 1) {
        pokeball->rotate("z", 15);
        pokeball->translate(point3(8, 0, 0));
        
        

    }
    else if (i > 1 && i <=2) {
        pokeball->rotate("z", 15);
        pokeball->translate(point3(6, 0, 0));
    }
    else if (i > 2 && i <=3) {
        pokeball->rotate("z", 15);
        pokeball->translate(point3(4, 0, 0));
    }
    else if (i > 4 && i <= 7) {
        pokeball->rotate("z", -15);
        pokeball->translate(point3(-8, 0, 0));

    }
    else if (i <= 11) {
        pokeball->rotate("z", -15);
        pokeball->translate(point3(-8, 0, 0));

    }
    // roll to wall
    else if (i > 13 && i <= 16) {
        pokeball->rotate("z", 15);
        pokeball->translate(point3(8, 0, 0));

    }
    else if (i <= 20) {
        pokeball->rotate("x", -15);
        pokeball->rotate("y", -15);
        pokeball->rotate("z", -15);
        pokeball->translate(point3(6, 0, 3));
    }
    else if (i <= 23) {
        pokeball->rotate("x", 15);
        pokeball->rotate("y", -15);
        //pokeball->rotate("z", -15);
        pokeball->translate(point3(-1, 0, -3));
    }
    else if (i <= 27) {
        pokeball->rotate("x", 11);
        pokeball->rotate("y", -13);
        //pokeball->rotate("z", -15);
        pokeball->translate(point3(-1, 0, -3));
    }
    else if (i <= 29) {
        pokeball->rotate("x", 9);
        pokeball->rotate("y", -10);
        //pokeball->rotate("z", -15);
        pokeball->translate(point3(-0.5, 0, -2));
    }
    else if (i <= 32) {
        pokeball->rotate("x", 5);
        pokeball->rotate("y", -7);
        //pokeball->rotate("z", -15);
        pokeball->translate(point3(-0.25, 0, -0.75));
    }
    else if (i <= 37) {
        // Start out at 0.5 and work up
        auto light = make_shared<diffuse_light>(color((0.95*(i-32)), (0.95*(i-32)), (0.85*(i-32))));
        world.add(make_shared<sphere>(point3(57.625, 28, 41.125), 4, light));
        // auto light = make_shared<diffuse_light>(color((-33+i), (-33+i), (-33+i)));
        // world.add(make_shared<sphere>(point3(84.375, 25, 47.375), 4, light));
        std::cout << pokeball->get_center().x() << std::endl;
        std::cout << pokeball->get_center().y() << std::endl;
        std::cout << pokeball->get_center().z() << std::endl;

    }

    // else if (i <= 16) {
    //     return world;
    // }
    
    
    
    return world;
}


hittable_list cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(12, 12, 12));

    shared_ptr<quad> green_wall = make_shared<quad>(point3(138.75,0,0), vec3(0,138.75,0), vec3(0,0,138.75), green);
    shared_ptr<quad> red_wall = make_shared<quad>(point3(0,0,0), vec3(0,138.75,0), vec3(0,0,138.75), red);
    shared_ptr<quad> light_quad = make_shared<quad>(point3(28.25,138.5,31.75), vec3(82.5,0,0), vec3(0,0,76.25), light);
    shared_ptr<quad> white_wall = make_shared<quad>(point3(138.75,138.75,138.75), vec3(-138.75,0,0), vec3(0,0,-138.75), white);
    shared_ptr<quad> white_floor = make_shared<quad>(point3(0,0,0), vec3(138.75,0,0), vec3(0,0,138.75), white);
    shared_ptr<quad> white_ceiling = make_shared<quad>(point3(0,0,138.75), vec3(138.75,0,0), vec3(0,138.75,0), white);



    world.add(green_wall);
    world.add(red_wall);
    world.add(light_quad);
    world.add(white_floor);
    world.add(white_wall);
    world.add(white_ceiling);

   
   return world;
}

hittable_list deconstructed_box(hittable_list world, int i, shared_ptr<quad> green_wall, shared_ptr<quad> red_wall, shared_ptr<quad> white_wall) {
    
    
    // Separate the quads of the box for the first 10 frames
    if (i > 0 && i < 10) {
        green_wall->translate(vec3(5,0,0));
        red_wall->translate(vec3(-5,0,0));
        white_wall->translate(vec3(0,0,5));
        world.add(green_wall);
        world.add(red_wall);
        world.add(white_wall);
        

    }

    // Put it back together for the next 10 frames
    if (i > 10) {
        green_wall->translate(vec3(-5,0,0));
        red_wall->translate(vec3(5,0,0));
        white_wall->translate(vec3(0,0,-5));
        world.add(green_wall);
        world.add(red_wall);
        world.add(white_wall);

    }


    return world;
}

hittable_list pokeball_roll_transformation(shared_ptr<sphere> sphere1, point3 floor_center, hittable_list world) {
    // Compute rotation
        double angle = 15.0 * M_PI / 180.0; //* i;
        double relative_x = sphere1->get_center().x() - floor_center.x();
        double relative_z = sphere1->get_center().z() - floor_center.z();

        double x_new = relative_x * cos(angle) - relative_z * sin(angle) + floor_center.x();
        double z_new = relative_x * sin(angle) + relative_z * cos(angle) + floor_center.z();
        
        double x_offset = x_new - sphere1->get_center().x();
        double y_offset = 0;  // We're not changing the y-coordinate for the orbit around y-axis.
        double z_offset = z_new - sphere1->get_center().z();

        if (std::find(world.objects.begin(), world.objects.end(), sphere1) != world.objects.end()) {
            world.remove(sphere1);
        }

        //sphere1 = make_shared<translate>(sphere1, point3(floor_center.x() + x_new, sphere1->get_center().y(), floor_center.z() + z_new));
        
        sphere1->translate(point3(x_offset, y_offset, z_offset));
        sphere1->rotate("y", 30);
        world.add(sphere1);

        return world;
}



int main() {

    clock_t timeStart = clock();

    // Image
    const auto aspect_ratio = 1.0; //16.0 / 9.0;
    const int image_width = 1000;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 900;
    const int max_depth = 50;
    



    auto world = cornell_box();



    // Mist layer
    //shared_ptr<hittable> mist = box(point3(0,0,0), point3(138.75, 138.75, 138.75), make_shared<lambertian>(color(0.73, 0.73, 0.73)));
    //world.add(make_shared<constant_medium>(mist, 0.0025, color(0.65, 0.65, 0.65)));
  

    //auto checker = make_shared<checker_texture>(4, color(.2, .3, .5), color(.9, .9, .9));

    //auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto pokeball_texture = make_shared<image_texture>("pokeball.jpg");
    // auto earth_surface = make_shared<lambertian>(earth_texture);
   auto metal_pokeball_surface = make_shared<metal>(pokeball_texture, 1.0);
    //auto pokeball_surface = make_shared<lambertian>(pokeball_texture);

    //auto globe = make_shared<sphere>(point3(0,0,0), 2, earth_surface);
    //auto globe = make_shared<sphere>(point3(0,0,0), 2, pokeball_surface);


    // world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    // world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    //auto metal1 = make_shared<metal>(color(0.8, 0.8, 0.9), 1.0);
    //shared_ptr<sphere> sphere1 = make_shared<sphere>(point3(109.375, 20, 69.375), 20, pokeball_surface);//make_shared<lambertian>(pertext));
    shared_ptr<sphere> metal_sphere1 = make_shared<sphere>(point3(69.375, 20, 69.375), 20, metal_pokeball_surface);//make_shared<lambertian>(pertext));
    metal_sphere1->rotate("y", 250);
    // metal_sphere1->rotate("z", 25);
    // metal_sphere1->rotate("x", -45);
    world.add(metal_sphere1);
    // Keeping track of the floor panel's center point for sphere orbiting
    //point3 floor_center = point3(69.375, 20, 69.375);


    point3 lookfrom(70, 70, -125);  // Adjusting the z-value to bring the camera closer.
    point3 lookat(70, 70, 0);

    //hittable_list world = day_and_night_setup();

    //hittable_list world;

    // auto red   = make_shared<lambertian>(color(.65, .05, .05));
    //     auto white = make_shared<lambertian>(color(.73, .73, .73));
    //     auto green = make_shared<lambertian>(color(.12, .45, .15));

    //     shared_ptr<quad> green_wall = make_shared<quad>(point3(138.75,0,0), vec3(0,138.75,0), vec3(0,0,138.75), green);
    //     shared_ptr<quad> red_wall = make_shared<quad>(point3(0,0,0), vec3(0,138.75,0), vec3(0,0,138.75), red);
    //     shared_ptr<quad> white_ceiling = make_shared<quad>(point3(138.75,138.75,138.75), vec3(-138.75,0,0), vec3(0,0,-138.75), white);
    //     shared_ptr<quad> white_floor = make_shared<quad>(point3(0,0,0), vec3(138.75,0,0), vec3(0,0,138.75), white);
    //     shared_ptr<quad> white_wall = make_shared<quad>(point3(0,0,138.75), vec3(138.75,0,0), vec3(0,138.75,0), white);

    // auto light = make_shared<diffuse_light>(color(12, 12, 12));

    // shared_ptr<quad> light_quad = make_shared<quad>(point3(28.25,138.5,31.75), vec3(82.5,0,0), vec3(0,0,76.25), light);

    // world.add(light_quad);

    //   world.add(green_wall);
    //     world.add(red_wall);
    //     world.add(white_floor);
    //     world.add(white_wall);
    //     world.add(white_ceiling);



    // auto sunlight = make_shared<diffuse_light>(color(1,1,0.9));
    // shared_ptr<sphere> sun = make_shared<sphere>(point3(300,300, 150.375), 180, sunlight);
    // world.add(sun);
    // world.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));

    //shared_ptr<sphere> glass_sphere = make_shared<sphere>(point3(79.375, 70, 29.375), 15, make_shared<dielectric>(1.5));//make_shared<lambertian>(pertext));
    //world.add(glass_sphere);
    //auto pertext = make_shared<noise_texture>(1);
    //world.add(make_shared<sphere>(point3(57, 67, 58), 25, make_shared<lambertian>(pertext)));



    vec3 vup(0,1,0);
    auto dist_to_focus = 15.0;
    auto aperture = 0.0;
    double vfov = 50; //20;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

    cam.background        = color(0, 0, 0);

    int frames = 40;


    // Loop to render three images with different rotations
    // View requirement
    for (int i = 0; i < frames; ++i) {

        //world = pokeball_roll_transformation(sphere1, floor_center, world);
        world = poke_ball_test(metal_sphere1, world, i);
        // world = day_and_night_loop(sun, world, i);

        // world = deconstructed_box(world, i, green_wall, red_wall, white_wall);

        
        

        // Open output file
        std::ofstream outFile("output" + std::to_string(i+1) + ".ppm");
        if (!outFile) {
            std::cerr << "Could not open output file" << std::endl;
            return 1;
        }
        std::string remaining_frames = "Frames remaining: " + std::to_string(frames-i);
        std::cout << remaining_frames << std::endl;
        // Render scene
        render_scene(outFile, cam, world, image_width, image_height, samples_per_pixel, max_depth);
    }

    // Output ray intersection data
    clock_t timeEnd = clock();
    printf("\n");
    printf("Render time                                   : %04.2f (sec)\n", (float)(timeEnd - timeStart) / CLOCKS_PER_SEC);
    printf("Total number of triangles                     : %llu\n", totalNumTris.load());
    printf("Total number of primary rays                  : %llu\n", numPrimaryRays);
    printf("Total number of ray-triangles tests           : %llu\n", numRayTrianglesTests.load());
    printf("Total number of ray-triangles intersections   : %llu\n", numRayTrianglesIsect.load());
    printf("Total number of Bounding Volume intersections : %llu\n", boundingVolumeIsect.load());
    printf("Total number of object intersections          : %llu\n", objectIsect.load());


    std::cerr << "\nDone.\n";
}


