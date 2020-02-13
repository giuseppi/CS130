varying vec4 position;
varying vec3 normal;
varying vec3 light_direction;

void main()
{
    vec4 ambient = vec4(1, 0, 0, 1);
    vec4 diffuse = vec4(0, 1, 0, 1);
    vec4 specular = vec4(0, 0, 1, 1);
  
    ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    
    for(int i = 0; i < g1_MaxLights; i++){
	diffuse += gl_LightSource[i].diffuse * gl_FrontMaterial.diffuse * max(dot(light_direction, normal), 0,0);
	vec4 reflVec = vec4(light_direction - 2.0*dot(light_direction ,normal)*normal,1);
	float specMath = max(dot(reflVec, position,0);
	specular += gl_LightSource[i].specular*gl_FrontMaterial.specular*pow(specMath, gl_FrontMaterial.shininess);
    } 

    gl_FragColor = ambient + diffuse + specular;
}
