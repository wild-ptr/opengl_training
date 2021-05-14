#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Model.h"
#include "Shader.h"

typedef struct
{
    vec3* vertices;
    vec3* surf_normals;
    vec3* tangents;
    vec2* texture_coords;
    unsigned int* indices;

    size_t num_of_vertices;
    size_t num_of_indices;

    Material material;
} ModelData;

static void modeldata_destroy(ModelData* d)
{
    if(d->vertices)
    {
        free(d->vertices);
        d->vertices = NULL;

        free(d->surf_normals);
        d->surf_normals = NULL;

        free(d->tangents);
        d->tangents = NULL;

    }

    if(d->texture_coords)
    {
        free(d->texture_coords);
        d->texture_coords = NULL;
    }

    if(d->indices)
    {
        free(d->indices);
        d->indices = NULL;
    }
}


static Texture load_material(
        const struct aiMaterial* material,
        enum aiTextureType type,
        const char* path_root)
{
    // get only first texture of a given type, if it exists, for now.
    if(aiGetMaterialTextureCount(material, type))
    {
        struct aiString str;
        aiGetMaterialTexture(material, type, 0, &str, NULL, NULL, NULL, NULL, NULL, NULL);
        const char* filepath = str.data;

        size_t path_size = strlen(filepath) + strlen(path_root) + 1;
        char* combined_path = malloc(sizeof(char) * path_size);
        strcpy(combined_path, path_root);
        strcat(combined_path, filepath);

        printf("Combined path of texture is: %s\n", combined_path);

        Texture t;
        texture_init(&t, combined_path);

        free(combined_path);

        return t;
    }
    else
    {
        Texture t;
        texture_zero_init(&t);
        return t;
    }
}

static void set_vertex_attributes_for_model()
{
    const size_t stride = (sizeof(vec3) * 3) + sizeof(vec2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(float)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(9*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
}

// this is ultra inefficient, as we just copy and paste data for no reason all the time.
// will rework this later. Gl can work with separate data buffers i guess.
static float* lay_out_data_for_vbo(ModelData* data)
{
    size_t stride = (sizeof(vec3) * 3) + sizeof(vec2);
    char* vertex_data = malloc(stride * data->num_of_vertices);
    for(int i = 0; i < data->num_of_vertices; ++i)
    {
        size_t offset = 0;

        memcpy(vertex_data + (stride * i), data->vertices[i], sizeof(vec3));
        offset += sizeof(vec3);

        memcpy(vertex_data + (stride * i) + offset, data->surf_normals[i], sizeof(vec3));
        offset += sizeof(vec3);

        memcpy(vertex_data + (stride * i) + offset, data->tangents[i], sizeof(vec3));
        offset += sizeof(vec3);

        memcpy(vertex_data + (stride * i) + offset, data->texture_coords[i], sizeof(vec2));
        offset += sizeof(vec2);
    }

    return (float*)vertex_data;
}

static Renderable create_renderable_from_data(ModelData* data)
{
    float* vertex_data = lay_out_data_for_vbo(data);
    size_t stride = (sizeof(vec3) * 3) + sizeof(vec2);

    printf("Mesh indices: %lu \n", data->num_of_indices);
    printf("Mesh vertices: %lu \n", data->num_of_vertices);
    //for(size_t i = 0; i < data->num_of_vertices; ++i)
    //{
    //    printf("%u, ", data->indices[i]);
    //}
    printf("\n");


    Renderable r = {};
    renderable_create(&r,
                      set_vertex_attributes_for_model,
                      vertex_data,
                      stride * data->num_of_vertices,
                      data->indices,
                      data->num_of_indices,
                      &data->material,
                      1,
                      NULL);

    //modeldata_destroy(data);
    //free(vertex_data);
    return r;
}

static Renderable process_mesh(
        const struct aiMesh* mesh,
        const struct aiScene* scene,
        const Model* model)
{
    ModelData data = {};

    if(!mesh->mNumVertices)
    {
        Renderable r = {};
        printf("no vertices in mesh. aborting.");
        return r;
    }

    data.num_of_vertices = mesh->mNumVertices;
    data.num_of_indices = mesh->mNumFaces * 3;

    data.vertices = malloc(sizeof(vec3) * mesh->mNumVertices);
    data.surf_normals = malloc(sizeof(vec3) * mesh->mNumVertices);
    data.indices = malloc(sizeof(unsigned int) * mesh->mNumFaces * 3); // triangle faces.
    data.tangents = malloc(sizeof(vec3) * mesh->mNumVertices);

    data.texture_coords = malloc(sizeof(vec2) * mesh->mNumVertices);

    for(size_t i = 0; i < mesh->mNumVertices; i++)
    {
        // pack vertices into vec3
        data.vertices[i][0] = mesh->mVertices[i].x;
        data.vertices[i][1] = mesh->mVertices[i].y;
        data.vertices[i][2] = mesh->mVertices[i].z;

        // normals
        data.surf_normals[i][0] = mesh->mNormals[i].x;
        data.surf_normals[i][1] = mesh->mNormals[i].y;
        data.surf_normals[i][2] = mesh->mNormals[i].z;

        // tangent and bitangent of triangle
        data.tangents[i][0] = mesh->mTangents[i].x;
        data.tangents[i][1] = mesh->mTangents[i].y;
        data.tangents[i][2] = mesh->mTangents[i].z;

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            data.texture_coords[i][0] = mesh->mTextureCoords[0][i].x;
            data.texture_coords[i][1] = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            data.texture_coords[i][0] = 0;
            data.texture_coords[i][1] = 0;
        }
    }

    printf("after mesh processing, vertices.\n");

    // for each triangle assimp creates a face, which consists of indices to draw.
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        struct aiFace face = mesh->mFaces[i];

        for(size_t j = 0; j < 3; ++j) // 3 indices per face.
        {
            data.indices[(i * 3) + j] = mesh->mFaces[i].mIndices[j];
        }
    }

    // load_material returns zero-inited textures for textures not found.
    if(mesh->mMaterialIndex >= 0 && model->directory != NULL)
    {
        const char* directory_root = model->directory;
        struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        data.material.diffuse_map = load_material(material, aiTextureType_DIFFUSE, directory_root);
        data.material.normal_map = load_material(material, aiTextureType_HEIGHT, directory_root);
        data.material.specular_map = load_material(material, aiTextureType_SPECULAR, directory_root);
    }
    // we have full data, how to initialize this now?
    return create_renderable_from_data(&data);
}

// it can hold many renderables! Not only one!
static void process_node(Model* model, const struct aiNode* node, const struct aiScene* scene)
{
    // process all the node's meshes (if any)
    //#pragma omp parallel for
    // TODO: get rid of this ugly hack for x meshes limit-per-renderable
    if(model->meshes_size >= MODEL_MAX_MESHES)
        return;

    unsigned int i;
    for(i = 0; i < node->mNumMeshes; ++i)
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes[i + model->meshes_size] = process_mesh(mesh, scene, model);
    }

    model->meshes_size += node->mNumMeshes;
    printf("meshes from node: %u, total meshes: %lu\n", i, model->meshes_size);

    // then do the same for each of its children
    for(i = 0; i < node->mNumChildren; i++)
    {
        printf("index is %u, num is %u\n", i, node->mNumChildren);
        process_node(model, node->mChildren[i], scene);
    }
}

void model_load(Model* model, const char* path, Shader* shader)
{
    const struct aiScene* scene = aiImportFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_GenSmoothNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph
        );

    printf("scene flags: %u\n", scene->mFlags);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Error while loading the model!");
    }

    const char* path_break = strchr(path, '/');
    if(path_break == NULL)
    {
        printf("Invalid path to model.\n");
    }

    while(true)
    {
        const char* temp = strchr(path_break + 1, '/');

        if(temp == NULL)
            break;

        path_break = temp;
    } // find last '/'

    ptrdiff_t size_to_copy = path_break - path + 1; // including the "/"

    char* dir = malloc(sizeof(char) * (size_to_copy + 1)); // +1 to account for \0
    strncpy(dir, path, size_to_copy);
    dir[size_to_copy] = '\0'; // strncpy does not null-terminate string if src > dest.

    model->directory = dir;

    printf("directory is: %s\n", model->directory);
    process_node(model, scene->mRootNode, scene);
}

void model_draw(Model* model)
{
    for(int i = 0; i < model->meshes_size; ++i)
    {
        renderable_draw_with_shader(&model->meshes[i], model->shader);
    }
}

void model_bind_shader(Model* m, Shader* s)
{
    if(m->shader == NULL)
        m->shader = malloc(sizeof(Shader));

    *m->shader = *s;
}

void model_bind_shader_data(Model* m, void* data)
{
    if(m->shader != NULL)
        shader_update_uniform_calc_data(m->shader, data);
}

