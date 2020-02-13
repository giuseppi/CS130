#include "mesh.h"
#include <fstream>
#include <string>
#include <limits>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e;
    vec3 v;
    box.Make_Empty();
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
            box.Include_Point(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }
    }
    number_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    double d = 0;
    if (part < 0) {    
    	for (size_t i = 0; i < triangles.size(); ++i) {
		if (Intersect_Triangle(ray, i, d)) {
            		Hit hit;
            		hit.object = this;
            		hit.dist = d;
            		hit.part = i;
            		return hit;
        	}
    	}
    	return {0,0,0};
    }
    else {
        if (Intersect_Triangle(ray, part, d)) {
            Hit hit;
            hit.object = this;
            hit.dist = d;
            hit.part = part;
            return hit;
        }
    	return {0,0,0};
    }

    return {};
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3& point, int part) const
{
    assert(part>=0);
    
    vec3 i = vertices[triangles[part][2]] - vertices[triangles[part][0]];
    vec3 j = vertices[triangles[part][1]] - vertices[triangles[part][0]];
    vec3 k = cross(j, i).normalized();
    
    return k;
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
bool Mesh::Intersect_Triangle(const Ray& ray, int tri, double& dist) const
{
    vec3 v = vertices[triangles[tri][1]] - vertices[triangles[tri][0]];
    vec3 w = vertices[triangles[tri][2]] - vertices[triangles[tri][0]];
    vec3 x = cross(ray.direction, w);
    double det = dot(v, x);
    
    if (det < small_t && det > -(small_t)) {
        return false;
    }
    
    double id = 1 / det;
    
    vec3 y = ray.endpoint - vertices[triangles[tri][0]];
    double w1 = dot(y, x) * id; 
    if ((w1 <= -(weight_tolerance)) || (w1 > 1.00001)) {
        return false;
    }
    
    vec3 z = cross(y, v);
    double w2 = dot(ray.direction, z) * id; 
    if ((w2 <= -(weight_tolerance)) || ((w1+w2) > 1.00001)) {
        return false;
    }
    
    dist = dot(w, z) * id; 
    if (dist > small_t) {
        return true;
    }
    
    return false;
}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const
{
    Box b;
    vec3 x, y, z;
    
    x = vertices[triangles[part][0]];
    y = vertices[triangles[part][1]];
    z = vertices[triangles[part][2]];

    b.lo[0] = std::min(std::min(x[0], y[0]), z[0]);
    b.lo[1] = std::min(std::min(x[1], y[1]), z[1]);
    b.lo[2] = std::min(std::min(x[2], y[2]), z[2]);

    b.hi[0] = std::max(std::max(x[0], y[0]), z[0]);
    b.hi[1] = std::max(std::max(x[1], y[1]), z[1]);
    b.hi[2] = std::max(std::max(x[2], y[2]), z[2]);  
    return b;
}
