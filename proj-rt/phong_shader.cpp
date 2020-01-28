#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
	vec3 color;
	vec3 ambient;
	vec3 lightDir;
	// TO DO; Determine the color
	
	// FORMULA:
	// I = I_a + I_d + I_s
        
        // Calculate ambient
	ambient = color_ambient * world.ambient_color * world.ambient_intensity;

	color += ambient;


	for (unsigned i = 0; i < world.lights.size(); ++i) {
		// Calculate the light's direction
		double ld;
		lightDir = (world.lights[i]->position - intersection_point);
		ld = lightDir.magnitude();

		// Now we need to check if the object is in shadow to determine
		// if we add diffuse or specular
		if (world.enable_shadows) {
			Ray sr;
			sr.direction = lightDir.normalized();
			sr.endpoint = intersection_point;
			
			Hit hit = world.Closest_Intersection(sr);
			if ((hit.dist * sr.direction).magnitude() <= (ld + .01)) { // If there is an object in the way, do not add diffuse and specular
				continue;
			}	
		}

		// Calculate diffuse
		vec3 diffuse;
		double a = std::max(dot(normal, lightDir.normalized()), 0.0);
		vec3 lc = world.lights[i]->Emitted_Light(lightDir);
		diffuse = (a * lc * color_diffuse);
		color += diffuse;
		
		// Calculate specular
		vec3 specular;
		vec3 reflect = lightDir.normalized() - (2 * dot(lightDir.normalized(), normal) * normal);
		a = std::max(dot(ray.direction, reflect), 0.0);
		specular = (pow(a, specular_power) * lc * color_specular);
		color += specular;

	}

	return color;
}
