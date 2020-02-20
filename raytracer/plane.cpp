#include "plane.h"
#include "ray.h"
#include <cfloat>
#include <limits>

// Intersect with the half space defined by the plane.  The plane's normal
// points outside.  If the ray starts on the "inside" side of the plane, be sure
// to record a hit with t=0 as the first entry in hits.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    // Two cases:
    // Case 1: Parallel/On top of plane
    // Case 2: Intersects plane
    
    if(dot(normal, ray.direction) == 0) { // Case 1
	return {0, 0, 0};
    }
    else { // Case 2
	Hit intersect;
	intersect.object = this;
	intersect.part = part;
	intersect.dist = (dot(normal, (x1 - ray.endpoint))) / (dot(normal, ray.direction));
	if (intersect.dist == 0) {
		return {0, 0, 0};
	}
	return intersect;
    }

    return {0,0,0};
}

vec3 Plane::Normal(const vec3& point, int part) const
{
    return normal;
}

// There is not a good answer for the bounding box of an infinite object.
// The safe thing to do is to return a box that contains everything.
Box Plane::Bounding_Box(int part) const
{
    Box b;
    b.hi.fill(std::numeric_limits<double>::max());
    b.lo=-b.hi;
    return b;
}