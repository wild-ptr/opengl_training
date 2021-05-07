#pragma once
#include <stdbool.h>
#include <stddef.h>

typedef struct GBuffer
{
    unsigned int gBuffer;
    unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedoSpec;
    unsigned int depthRenderbuffer;
} GBuffer;

bool gbuffer_init(GBuffer* buf, size_t width, size_t height);
void gbuffer_use(GBuffer* buf);
void gbuffer_destroy(GBuffer* buf);
