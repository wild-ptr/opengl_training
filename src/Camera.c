#include "Camera.h"
#include <string.h>

void camera_init(Camera* cam)
{
    vec3 zeros = {0.0f, 0.0f, 0.0f};
    memcpy(cam->pos_v, zeros, sizeof(zeros));
    memcpy(cam->front_v, zeros, sizeof(zeros));
    memcpy(cam->up_v, zeros, sizeof(zeros));
}
