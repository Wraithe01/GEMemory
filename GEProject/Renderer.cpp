#include "Renderer.h"

void RendererInit(uint32_t width, uint32_t height)
{
    //state->width = width;
    //state->height = height;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(width, height, "Project");

    BeginBlendMode(BLEND_ALPHA);

    //RendererCalcScale(width, height);

    camera.position = { 0.0f, 0.0f, 0.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    //SetCameraMode(camera, CAMERA_FREE);

    SetTargetFPS(60);
}