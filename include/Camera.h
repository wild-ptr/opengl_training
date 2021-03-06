#pragma once
#include <cglm/cglm.h>

typedef struct GLFWwindow GLFWwindow;

typedef struct Camera
{
    vec3 pos_v;
    vec3 dir_v;
    vec3 up_v;
    float time_d; // @TODO: camera shouldnt know about time delta.
    float cam_speed;
    float sensitivity;
    float last_x;
    float last_y;
    float pitch;
    float yaw;
} Camera;

enum CameraDirections
{
    CAMERA_FORWARD,
    CAMERA_BACKWARD,
    CAMERA_LEFT,
    CAMERA_RIGHT
};

void camera_init(Camera*);
void camera_move(Camera*, enum CameraDirections); // this will need an interface change
void camera_move_direction_clbk(GLFWwindow*, double, double);

void camera_set_current(Camera*);
