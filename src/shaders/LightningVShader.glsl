#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

uniform mat4 model_m;
uniform mat4 view_m;
uniform mat4 proj_m;
uniform mat3 norm_m;

out vec3 Norm;
out vec3 FragPosViewS;

void main()
{
    gl_Position = proj_m * view_m * model_m * vec4(pos, 1.0);
    // inversion is very costly for shaders
    // and whole normal transposition matrix
    // should be done on CPU and passed as uniform.
    //Norm = mat3(transpose(inverse(model_m))) * norm;
    Norm = norm_m * norm;
    FragPosViewS = (model_m * vec4(pos, 1.0)).xyz;
}
