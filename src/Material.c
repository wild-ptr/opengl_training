#include "Material.h"

Material* material_init(Material* material,
                        const char* path, unsigned int texture_type,
                        const char* path_specular, unsigned int spec_tex_type)
{
    texture_init_rgb(&material->diffuse_map, path, texture_type);
    texture_init_rgb(&material->specular_map, path_specular, spec_tex_type);
    return material;
}
