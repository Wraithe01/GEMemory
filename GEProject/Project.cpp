// This is the main folder where "Run" is the entrypoint of the program

#include "project.h"
#include "Renderer.h"
#include "Includes.h"

#include <raylib.h>
//#include <imgui.h>

//#include "Imgui.h"

void Run()
{
    //- Initialization
    RendererInit(1920, 1080); // This must be working before imgui can work
    //ImguiInit();

    //- Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

    //    // InputUpdate();
    }
    //- De-Initialization

    //RaylibImGui::Deinit();
    //ImguiDeInit();
    CloseWindow();          // Close window and OpenGL context     
}