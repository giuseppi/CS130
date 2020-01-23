#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    vec3 a = ray.endpoint - center;

    float val1 = dot(ray.direction, ray.direction);
    float val2 = 2 * dot(ray.direction, a);
    float val3 = dot(a,a) - (radius * radius);
    float val4 = (val2 * val2) - (4 * (val1 * val3));

    if (val4 < 0) {
	return {0, 0, 0}; // This means no intersection
    }

    // Else, there is an intersection

    float point1 = ((-1 * val2) - sqrt(val4)) / (val1 * 2); // First intersection point
    float point2 = ((-1 * val2) + sqrt(val4)) / (val1 * 2); // Possible exiting intersection point
    
    if (point1 < 0) {
	if (point2 > 0) {
		point1 = point2;
	}
	else {
		return {0, 0, 0};
	}
    }

    Hit intersect;
    
    if (val4 > 0) { // Ray enters through the sphere; two intersections
	intersect.object = this;
	intersect.dist = point1;
	intersect.part = part;
	return intersect;
    }
    else if (val4 == 0) { // Ray hits the sphere; one intersection
	intersect.object = this;
	intersect.dist = point1;
	intersect.part = part;
	return intersect;	
    }

    return {0,0,0};
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal;
    TODO; // compute the normal direction
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
