#include "Camera.h"
#include <string.h>

static Camera* current_camera;

void camera_move_direction_clbk(GLFWwindow* window, double xpos, double ypos)
{
    static bool first_mouse_input = false;
    Camera* cam = current_camera;

    if (first_mouse_input)
    {
        cam->last_x = xpos;
        cam->last_y = ypos;
        first_mouse_input = true;
    }

    float xoffset = xpos - cam->last_x;
    float yoffset = cam->last_y - ypos;
    cam->last_x = xpos;
    cam->last_y = ypos;

    xoffset *= cam->sensitivity;
    yoffset *= cam->sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if(cam->pitch > 89.0f)
        cam->pitch = 89.0f;
    if(cam->pitch < -89.0f)
        cam->pitch = -89.0f;

    vec3 dir;
    dir[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    dir[1] = sin(glm_rad(cam->pitch));
    dir[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    glm_normalize(dir);

    memcpy(cam->dir_v, &dir, sizeof(vec3));
}

void camera_set_current(Camera* cam)
{
    current_camera = cam;
}

void camera_init(Camera* cam)
{
    vec3 zeros = {0.0f, 0.0f, 0.0f};
    memcpy(cam->pos_v, zeros, sizeof(zeros));
    memcpy(cam->dir_v, zeros, sizeof(zeros));
    memcpy(cam->up_v, zeros, sizeof(zeros));
    cam->time_d = 0;
    cam->cam_speed = 0;
    cam->sensitivity = 0.1f;
    cam->pitch = 0.0f;
    cam->yaw = -90.0f;
}

void camera_move(Camera* cam, enum CameraDirections dir)
{
    vec3 scaled;
    switch(dir)
    {
        case CAMERA_FORWARD:
            memset(scaled, 0, sizeof(vec3));
            glm_vec3_scale(cam->dir_v, cam->cam_speed, scaled);
            glm_vec3_add(cam->pos_v, scaled, cam->pos_v);
            break;
        case CAMERA_BACKWARD:
            memset(scaled, 0, sizeof(vec3));
            glm_vec3_scale(cam->dir_v, cam->cam_speed, scaled);
            glm_vec3_sub(cam->pos_v, scaled, cam->pos_v);
            break;
        case CAMERA_LEFT:
            memset(scaled, 0, sizeof(vec3));
            glm_vec3_cross(cam->dir_v, cam->up_v, scaled);
            glm_normalize(scaled);
            glm_vec3_scale(scaled, cam->cam_speed, scaled);
            glm_vec3_sub(cam->pos_v, scaled, cam->pos_v);
            break;
        case CAMERA_RIGHT:
            memset(scaled, 0, sizeof(vec3));
            glm_vec3_cross(cam->dir_v, cam->up_v, scaled);
            glm_normalize(scaled);
            glm_vec3_scale(scaled, cam->cam_speed, scaled);
            glm_vec3_add(cam->pos_v, scaled, cam->pos_v);
            break;
    }
}
