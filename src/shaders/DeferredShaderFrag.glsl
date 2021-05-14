#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float att_constant;
    float att_linear;
    float att_quadratic;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float att_constant;
    float att_linear;
    float att_quadratic;
    
    float inner_cone_cosine;
    float outer_cone_cosine;
};

in vec3 FragPos_world;
in mat3 TBN;
in vec2 texCoords;
out vec4 FragColor;
  

uniform Material material_0;
uniform Material material_1;
uniform vec3 viewPos;
uniform float time;

uniform Light omniLights[10];
uniform int omniLightsSize;

uniform SpotLight spotLights[10];
uniform int spotLightsSize;

vec3 calculateOmniLights(vec3 fragmentNormal, vec3 diffuseColor, vec3 specularFactor, float shininess)
{
    vec3 ambient_sum = vec3(0.0);
    vec3 diffuse_sum = vec3(0.0);
    vec3 specular_sum = vec3(0.0);

    for(int i = 0; i < omniLightsSize; ++i)
    {
        //ambient component
        ambient_sum += diffuseColor * omniLights[i].ambient;

        // diffuse component
        vec3 norm_n = normalize(fragmentNormal);
        vec3 light_dir = normalize(omniLights[i].position - FragPos_world);
        diffuse_sum += max(dot(norm_n, light_dir), 0.0) * omniLights[i].diffuse * diffuseColor;

        // specular component
        // so we get the view direction vector.
        vec3 view_dir = normalize(viewPos - FragPos_world);
        // and we need to get reflected ray. Since omniLights.dir 
        // points towards the cube, we need to negate vector before reflect.
        vec3 reflect_dir = reflect(-light_dir, norm_n);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
        specular_sum += spec * specularFactor * omniLights[i].specular;
    }

    return vec3(ambient_sum + diffuse_sum + specular_sum);
}

float getAttenuationFactor(float att_const, float att_linear, float att_quadratic, float dist)
{
    return (1/(att_const + (att_linear * dist) + (att_quadratic * dist * dist)));
}

vec3 calculateSpotLights(vec3 fragmentNormal, vec3 diffuseColor, vec3 specularFactor, float shininess)
{
    vec3 ambient_sum = vec3(0.0);
    vec3 diffuse_sum = vec3(0.0);
    vec3 specular_sum = vec3(0.0);
    
    vec3 color = vec3(0.0);
    
    for(int i = 0; i < spotLightsSize; ++i)
    {
        // first, we need to determine whether the fragment is in the light cone, or not.
        vec3 normal_light_dir = normalize(-spotLights[i].direction);
        vec3 frag_light_dir = normalize(spotLights[i].position - FragPos_world);
        float theta = max(dot(frag_light_dir, normal_light_dir), 0);

        if(theta > spotLights[i].outer_cone_cosine) // if it holds, it is in cone.
        {
            // calculate epsilon of outer-inner and substitute that for edge intensity formula
            float epsilon = spotLights[i].inner_cone_cosine - spotLights[i].outer_cone_cosine;
            float intensity = clamp((theta - spotLights[i].outer_cone_cosine) / epsilon, 0.0, 1.0);    

            float attenuation = getAttenuationFactor(spotLights[i].att_constant,
                                                     spotLights[i].att_linear,
                                                     spotLights[i].att_quadratic,
                                                     distance(FragPos_world, spotLights[i].position));

            // ambient component
            ambient_sum += diffuseColor * spotLights[i].ambient;
            ambient_sum *= intensity * attenuation;

            //diffuse component
            vec3 norm_n = normalize(fragmentNormal);
            vec3 light_dir = normalize(spotLights[i].position - FragPos_world);
            diffuse_sum += max(dot(norm_n, light_dir), 0.0) * spotLights[i].diffuse * diffuseColor;
            diffuse_sum *= intensity * attenuation;

            // specular component
            // so we get the view direction vector.
            vec3 view_dir = normalize(viewPos - FragPos_world);
            // and we need to get reflected ray. Since omniLights.dir 
            // points towards the fragment, we need to negate vector before reflect.
            vec3 reflect_dir = reflect(-light_dir, norm_n);
            float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
            specular_sum += spec * specularFactor * spotLights[i].specular;
            specular_sum *= intensity * attenuation;
        }
    }
    
    return vec3(ambient_sum + diffuse_sum + specular_sum);
}


void main()
{
    vec3 diffuseColor = vec3(texture(material_0.diffuse, texCoords));
    vec3 specularFactor = vec3(texture(material_0.specular, texCoords));
    vec3 normalMap = vec3(texture(material_0.normal, texCoords));

    normalMap = normalize(normalMap * 2.0 - 1.0);
    vec3 normal = (TBN * normalMap);
    
    vec3 omniComponent = calculateOmniLights(normal, diffuseColor, specularFactor, material_0.shininess);
    vec3 spotComponent = calculateSpotLights(normal, diffuseColor, specularFactor, material_0.shininess);

	FragColor = vec4((omniComponent + spotComponent), 1.0f);
}
