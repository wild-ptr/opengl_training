#include "Texture.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#ifndef TEXCACHE_NOT_FOUND
#define TEXCACHE_NOT_FOUND INT_MAX
#endif

typedef struct TextureRecord
{
    const char* path;
    unsigned int id;
} TextureRecord;

typedef struct TextureCache
{
    TextureRecord records[500];
    size_t tex_size;
} TextureCache;

static TextureCache tex_cache;


static unsigned int find_in_texcache(const char* path)
{
    for(size_t i = 0; i < tex_cache.tex_size; ++i)
    {
        if(strcmp(path, tex_cache.records[i].path) == 0)
        {
            return tex_cache.records[i].id;
        }
    }
    return TEXCACHE_NOT_FOUND;
}

static void add_to_texcache(const char* path, unsigned int id)
{
    if(tex_cache.tex_size < 500)
    {
        char* path_copy = malloc(sizeof(char) * strlen(path) + 1);
        strcpy(path_copy, path);

        tex_cache.records[tex_cache.tex_size].path = path_copy;
        tex_cache.records[tex_cache.tex_size].id = id;

        tex_cache.tex_size += 1;
    }
    else
    {
        printf("ERROR: texture cache overfilled, > 500 elems.\n");
        return;
    }
}

bool texture_init(Texture* tex, const char* path)
{
    // if it is already loaded:
    int tex_id = find_in_texcache(path);
    if(tex_id != TEXCACHE_NOT_FOUND)
    {
        tex->id = tex_id;
        tex->valid = true;
        return true;
    }

    // else lets load the texture!
    stbi_set_flip_vertically_on_load(true);
    int width, height, nr_channels;
    unsigned char* data = stbi_load(path, &width, &height, &nr_channels, 0);

    if (!data)
    {
        tex->valid = false;
        printf("Failed to load texture file: %s\n", path);
        return false;
    }

    GLenum texture_type;
    if(nr_channels == 1)
        texture_type = GL_RED;
    else if(nr_channels == 3)
        texture_type = GL_RGB;
    else if(nr_channels == 4)
        texture_type = GL_RGBA;

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

    add_to_texcache(path, tex->id);

    tex->valid = true;
    return true;
}

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

void texture_use_texunit(const Texture* tex, unsigned int active_texture)
{
    if(!tex->valid)
    {
        return;
    }
    glActiveTexture(active_texture);
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

Texture* texture_zero_init(Texture* tex)
{
    tex->id = 0;
    tex->valid = false;
}
