#version 330 core
in vec2 TextureCoords;
out vec4 FragColor;

// this basically takes texture from currently bound GL_TEXTURE_2D
uniform sampler2D usedTexture;
uniform sampler2D usedTexture2;
uniform float blendFactor;
void main()
{
    FragColor = mix(texture(usedTexture, TextureCoords),
                    texture(usedTexture2, TextureCoords),
                    blendFactor);
}
