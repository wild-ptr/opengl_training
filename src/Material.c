#include "Material.h"
#include <glad/glad.h>

Material* material_init(Material* material,
                        const char* path, unsigned int texture_type,
                        const char* path_specular, unsigned int spec_tex_type,
                        const char* path_norm, unsigned int norm_tex_type)
{
    if(path)
        texture_init_rgb(&material->diffuse_map, path, texture_type);
    if(path_specular)
        texture_init_rgb(&material->specular_map, path_specular, spec_tex_type);
    if(path_norm)
        texture_init_rgb(&material->normal_map, path_norm, norm_tex_type);

    return material;
}

void material_use(Material* mat, unsigned int texunit_offset)
{
    // those accept invalid textures as noop, no need to check.
    texture_use_texunit(&mat->diffuse_map, GL_TEXTURE0 + texunit_offset);
    texture_use_texunit(&mat->specular_map, GL_TEXTURE0 + texunit_offset + 1);
    texture_use_texunit(&mat->normal_map, GL_TEXTURE0 + texunit_offset + 2);
}
