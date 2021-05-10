#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "Shader.h"

typedef struct
{
    vec3* vertices;
    vec3* surf_normals;
    vec2* texture_coords;
    unsigned int* indices;

    Material material;
} mesh_data;

static Renderable process_mesh(const struct aiMesh* mesh, const struct aiScene* scene)
{
    mesh_data data;
    data.vertices = malloc(sizeof(vec3) * mesh->mNumVertices);
    data.surf_normals = malloc(sizeof(vec3) * mesh->mNumVertices);
    data.indices = malloc(sizeof(unsigned int) * mesh->mNumFaces * 3); // triangle faces.

    if(mesh->mTextureCoords[0]);
        data.surf_normals = malloc(sizeof(vec3) * mesh->mNumVertices);

    for(size_t i = 0; i < mesh->mNumVertices; i++)
    {
        // pack vertices into vec3
        data.vertices[i][0] = mesh->mVertices[i].x;
        data.vertices[i][1] = mesh->mVertices[i].y;
        data.vertices[i][2] = mesh->mVertices[i].y;

        // normals
        data.surf_normals[i][0] = mesh->mNormals[i].x;
        data.surf_normals[i][1] = mesh->mNormals[i].y;
        data.surf_normals[i][2] = mesh->mNormals[i].z;

        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            data.texture_coords[i][0] = mesh->mTextureCoords[0][i].x;
            data.texture_coords[i][1] = mesh->mTextureCoords[0][i].y;
            data.texture_coords[i][2] = mesh->mTextureCoords[0][i].z;
        }
        else
            memset(&data.texture_coords[i], 0, sizeof(vec2));
    }

    // for each triangle assimp creates a face, which consists of indices to draw.
    for(size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        for(size_t j = 0; j < 3; ++j)
        {
            data.indices[(i * 3) + j] = mesh->mFaces[i].mIndices[j];
        }
    }
}

static void process_node(Model* model, struct aiNode* node, const struct aiScene* scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        model->meshes[i] = process_mesh(mesh, scene);
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        process_node(model, node->mChildren[i], scene);
    }
}

void model_load(Model* model, const char* path, Shader* shader)
{
    const struct aiScene* scene = aiImportFile(path,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipUVs |
        aiProcess_GenNormals |
        aiProcess_OptimizeMeshes);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        return;
    }
}

void model_draw(Model* model)
{
    for(int i = 0; i < model->meshes_size; ++i)
    {
        renderable_draw(&model->meshes[i]);
    }
}
