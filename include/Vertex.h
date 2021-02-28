#pragma once

typedef union Float2Vec
{
    float arr[2];
    struct xy{
        float x;
        float y;
    } xy;
} Float2Vec;

typedef union Float3Vec
{
    float arr[3];

    struct xyz{
    float x;
    float y;
    float z;
    } xyz;
} Float3Vec;

typedef union Float4Vec
{
    float arr[4];

    struct xyzw{
    float x;
    float y;
    float z;
    float w;
    } xyzw;
} Float4Vec;

typedef struct Vertex
{
    Float3Vec position;
    Float4Vec color;
    Float2Vec texture;
} Vertex;

