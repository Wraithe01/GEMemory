#include "project.h"

#include <raylib.h>
#include <imgui.h>

#include "Imgui.h"

void Run()
{
    //- Initialization

    //RendererInit(&state, 1920, 1080);
    ImguiInit();


    //- Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // InputUpdate();
    }



    //- De-Initialization

    ImguiDeInit();
    // CloseWindow();        // Close window and OpenGL context
}