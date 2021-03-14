#version 330 core
out vec4 FragColor;


struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material cubeMaterial;

void main()
{
	FragColor = vec4(cubeMaterial.diffuse, 1.0);
}
