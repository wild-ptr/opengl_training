#version 330 core
in vec3 Norm;
in vec3 FragPosViewS;

out vec4 FragColor;
  
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main()
{
    // ambient component
    vec3 ambientLight = material.ambient * light.ambient;
    
    // diffuse component
    vec3 norm_n = normalize(Norm);
    vec3 light_dir = normalize(light.position - FragPosViewS);

    // seems that max is usual with dot product to clamp it to 0.0
    vec3 diffuseLight = max(dot(Norm, light_dir), 0.0) * light.diffuse * material.diffuse;

    // specular component
    // so we get the view direction vector.
    vec3 view_dir = normalize(viewPos - FragPosViewS);
    // and we need to get reflected ray. Since light_dir 
    // points towards the cube, we need to negate vector before reflect.
    vec3 reflect_dir = reflect(-light_dir, norm_n);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specularLight = spec * material.specular * light.specular;

	FragColor = vec4((ambientLight + diffuseLight + specularLight), 1.0f);
}
