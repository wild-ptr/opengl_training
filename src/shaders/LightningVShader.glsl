#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 model_m;
uniform mat4 view_m;
uniform mat4 proj_m;
uniform mat3 norm_m;

out vec3 FragPos_world;
out vec2 texCoords;
out mat3 TBN;

void main()
{
    gl_Position = proj_m * view_m * model_m * vec4(pos, 1.0);

    // inversion is very costly for shaders
    // and whole normal transposed-inversed matrix
    // should be done on CPU and passed as uniform.

    //Norm = mat3(transpose(inverse(model_m))) * norm;
    FragPos_world = (model_m * vec4(pos, 1.0)).xyz;
    texCoords = aTexCoords;

    
    // tangent space calculations
    vec3 bitangent = cross(norm, tangent);

    vec3 T = normalize(norm_m * tangent);
    vec3 B = normalize(norm_m * bitangent);
    vec3 N = normalize(norm_m * norm);
    
    TBN = mat3(T, B, N);
}
