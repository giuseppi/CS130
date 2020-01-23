#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool disable_hierarchy;

Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    Hit closest = {0, 0, 0};
    closest.dist = 9999999999;
    
    for(unsigned i = 0; i < objects.size(); i++) {
      Hit t = objects[i]->Intersection(ray, -1);
      if ((t.dist < closest.dist) && (t.dist > small_t)) {
        closest.object = objects[i];
        closest.part = t.part;
        closest = t;
      }
    }
    
    return closest;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    /*
 *     end_point: camera position (from camera class)
 *         direction: a unit vector from the camera position to the world position of the pixel.
 *             vec3 class has normalized() function that returns the normalized vector;
 *                 e.g. (v1-v2).normalized()
 *                     */ 
    // set up the initial view ray here
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.position - camera.World_Position(pixel_index)).normalized();
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    if(!disable_hierarchy)
        Initialize_Hierarchy();

    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{
    // determine the color here
    //     /* 
    //         Get the closest hit with an object using Closest_Intersection
    //             if (there is an intersection) {
    //                   Set color using the object Shade_Surface function which calculates and
    //                         returns the color of the ray/object intersection point.
    //                               Shade_Surface receives as parameters: ray, intersection point, normal at
    //                                     the intersection point and recursion_depth. You can get the intersection
    //                                           point using the ray object and the normal using the object pointer
    //                                                 inside the hit object.
    //                                                     }
    //                                                         else (If there is no intersection) {
    //                                                               Use background_shader of the render_world class. The background shader
    //                                                                     is a flat_shader so you can use any 3d vector as parameters.
    //                                                                         }
    //                                                                             */ 
    vec3 color;
    Hit closest;
    closest = Closest_Intersection(ray);
    if(closest.object) {
      vec3 intersection_point = ray.Point(closest.dist);
      color = closest.object->material_shader->Shade_Surface(ray, intersection_point, closest.object->Normal(intersection_point, closest.part), recursion_depth);
    }
    else {
      vec3 no_intersection;
      color = background_shader->Shade_Surface(ray, no_intersection, no_intersection, recursion_depth);
    }
    return color;
}

void Render_World::Initialize_Hierarchy()
{
    TODO; // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
