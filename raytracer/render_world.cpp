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
    closest.dist = 9999999999999;
    //unsigned a = -1;
    std::vector<int> v;
    
    hierarchy.Intersection_Candidates(ray, v);
    
    for (unsigned i = 0; i < v.size(); ++i) {
	if (v[i] >= (int)hierarchy.entries.size()) {
		continue;	
	}
	Hit t = hierarchy.entries[v[i]].obj->Intersection(ray, hierarchy.entries[v[i]].part);
        if ((t.dist < closest.dist) && (t.dist > small_t)) {
            //a = v[i];
            closest = t;
        }
    }    
    /*
    for(unsigned i = 0; i < objects.size(); i++) {
      Hit t = objects[i]->Intersection(ray, -1);
      if ((t.dist < closest.dist) && (t.dist > small_t)) {
        closest.object = objects[i];
        closest.part = t.part;
        closest = t;
      }
    }
    */
    return closest;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    Ray ray;
    ray.endpoint = camera.position;
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();
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
    if (recursion_depth > recursion_depth_limit) {
        vec3 n;
        return background_shader->Shade_Surface(ray, n, n, recursion_depth);
    }

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
    // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.
    
    for (size_t i = 0; i < objects.size(); ++i) {
	for (int j = 0; j < objects[i]->number_parts; ++j) {
		Entry e;
		e.obj = objects[i];
		e.part = j;
		e.box = objects[i]->Bounding_Box(j);
		hierarchy.entries.push_back(e);
	}
    }

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
