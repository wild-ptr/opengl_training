#version 330 core
in vec3 Norm;
in vec3 FragPosViewS;

out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    // ambient component
    float ambientFactor = 0.1;
    vec3 ambientLight = ambientFactor * lightColor;
    
    // diffuse component
    vec3 norm_n = normalize(Norm);
    vec3 light_dir = normalize(lightPos - FragPosViewS);

    // seems that max is usual with dot product to clamp it to 0.0
    vec3 diffuseLight = max(dot(Norm, light_dir), 0.0) * lightColor;

    // specular component
    float specularFactor = 0.5;
    // so we get the view direction vector.
    vec3 view_dir = normalize(viewPos - FragPosViewS);
    // and we need to get reflected ray. Since light_dir 
    // points towards the cube, we need to negate vector before reflect.
    vec3 reflect_dir = reflect(-light_dir, norm_n);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specularLight = spec * specularFactor * lightColor;

	FragColor = vec4((ambientLight + diffuseLight + specularLight) * objectColor, 1.0f);
}
