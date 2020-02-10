#include <limits>
#include "box.h"

// Return whether the ray intersects this box.
bool Box::Intersection(const Ray& ray) const
{
    vec3 id;
    for (size_t i = 0; i < 3; i++) {
        id[i] = 1.0 / ray.direction[i];
    }
    double xtlo = (lo[0] - ray.endpoint[0]) * id[0];
    double xthi = (hi[0] - ray.endpoint[0]) * id[0];

    double ytlo = (lo[1] - ray.endpoint[1]) * id[1];
    double ythi = (hi[1] - ray.endpoint[1]) * id[1];
    
    double ztlo = (lo[2] - ray.endpoint[2]) * id[2];
    double zthi = (hi[2] - ray.endpoint[2]) * id[2];
    
    double t_min = std::max(std::max(std::min(xtlo, xthi), std::min(ytlo, ythi)), std::min(ztlo, zthi));
    double t_max = std::min(std::min(std::max(xtlo, xthi), std::max(ytlo, ythi)), std::max(ztlo, zthi));

    if (t_max < 0.0001) {
        return false;
    }
    
    if (t_min > (t_max + 0.0001)) { // no intersection
        return false;
    }
    
    return true;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box;

    box.lo[0] = std::min(this->lo[0], bb.lo[0]);
    box.lo[1] = std::min(this->lo[1], bb.lo[1]);
    box.lo[2] = std::min(this->lo[2], bb.lo[2]);

    box.hi[0] = std::max(this->hi[0], bb.hi[0]);
    box.hi[1] = std::max(this->hi[1], bb.hi[1]);
    box.hi[2] = std::max(this->hi[2], bb.hi[2]);

    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    if (pt[0] > hi[0]) {
    	hi[0] = pt[0];
    }
    if (pt[0] < lo[0]) {
    	lo[0] = pt[0];
    }

    if (pt[1] > hi[1]) {
    	hi[1] = pt[1];
    }
    if (pt[1] < lo[1]) {
    	lo[1] = pt[1];
    }

    if (pt[2] > hi[2]) {
    	hi[2] = pt[2];
    }
    if (pt[2] < lo[2]) {
    	lo[2] = pt[2];
    } 
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}
