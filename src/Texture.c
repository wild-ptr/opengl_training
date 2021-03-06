#include "Texture.h"
#include "stb_image.h"
#include <glad/glad.h>

bool texture_init_rgb(Texture* tex, const char* path, unsigned int texture_type)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nr_channels;
    unsigned char* data = stbi_load(path, &width, &height, &nr_channels, 0);

    if (!data)
    {
        tex->valid = false;
        printf("Failed to load texture file: %s\n", path);
        return false;
    }

    glGenTextures(1, &(tex->id));
    glBindTexture(GL_TEXTURE_2D, tex->id);

    // parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // first zero is mipmap level, we generate base texture, so 0.
    // second zero parameter is legacy and should always be set to zero.
    glTexImage2D(GL_TEXTURE_2D, 0, texture_type, width, height, 0,
                 texture_type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    tex->valid = true;
    return true;
}

void texture_use(Texture* tex)
{
    if(!tex->valid)
    {
        printf("Trying to use invalid texture.\n");
        return;
    }

    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void texture_use_texunit(Texture* tex, unsigned int active_texture)
{
    if(!tex->valid)
    {
        printf("Trying to use invalid texture.\n");
        return;
    }
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glActiveTexture(active_texture);
}
