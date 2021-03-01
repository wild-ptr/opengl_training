#pragma once
#include <cglm/cglm.h>
typedef struct Camera
{
    vec3 pos_v;
    vec3 front_v;
    vec3 up_v;
    long time_d;
    float cam_speed;
} Camera;

void camera_init(Camera*);

