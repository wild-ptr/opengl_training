#include "GBuffer.h"
#include <glad/glad.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

bool gbuffer_init(GBuffer* buf, size_t width, size_t height)
{
    glGenFramebuffers(1, &buf->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, buf->gBuffer); // unbind later.

    // - position color buffer
    glGenTextures(1, &buf->gPosition);
    glBindTexture(GL_TEXTURE_2D, buf->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buf->gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &buf->gNormal);
    glBindTexture(GL_TEXTURE_2D, buf->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, buf->gNormal, 0);

    // - color + specular color buffer
    glGenTextures(1, &buf->gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, buf->gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, buf->gAlbedoSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &buf->depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buf->depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // add renderbuffer attachment as 24 bit depth, 8 bit stencil
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
            buf->depthRenderbuffer);

    bool ret = true;
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("ERROR: GBuffer creation failed.\n");
        ret = false;
    }

cleanup:
    // bind default ones, @TODO: later on improve to read current ones and restore them.
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return ret;

} // whew! That was a mouthful.

void gbuffer_use(GBuffer* buf)
{
    glBindFramebuffer(GL_FRAMEBUFFER, buf->gBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, buf->depthRenderbuffer); // im not sure this is needed.
}
