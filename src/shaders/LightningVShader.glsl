#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 model_m;
uniform mat4 view_m;
uniform mat4 proj_m;

void main()
{
    gl_Position = proj_m * view_m * model_m * vec4(pos, 1.0);
}
