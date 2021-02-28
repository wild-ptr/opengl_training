#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

out vec4 VertexColor;
out vec2 TextureCoords;

uniform mat4 model_m;
uniform mat4 view_m;
uniform mat4 proj_m;

void main()
{
    gl_Position = proj_m * view_m * model_m * vec4(pos, 1.0);
    TextureCoords = texCoords;
}
