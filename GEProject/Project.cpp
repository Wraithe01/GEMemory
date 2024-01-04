// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "project.h"
#include "Renderer.h"


void Run()
{
    //- Initialization
    RendererInit(1920, 1080);  // This must be working before imgui can work
    ImguiInit();

    // Basic scene
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    Scene            scene;
    //  ZIP
    scene.AppendGUID("7023d2a2-6f6d-49fc-9512-c1a3bc78b278");
    scene.AppendGUID("798cb1b8-dfb9-4453-b2f7-ae67f75556e");
    scene.AppendGUID("8baa3bc4-1b74-4015-971c-a91073603842");
    scene.AppendGUID("bf2e0941-ad77-4d57-bd74-b7d43dc828ff");
    // TAR
    scene.AppendGUID("2fdb31c4-8760-4343-a8fa-5786c29ddb");
    scene.AppendGUID("405c6f96-eb47-4de5-8976-adeaa2800");
    scene.AppendGUID("61182300-5330-4425-8b0-fb3ba8e0c13a");
    scene.AppendGUID("8c696454-7ac0-47de-8490-c25d6d4b01c");

    // Load scene
    auto asyncRequest = resourceManager.LoadScene(scene);
    resourceManager.AsynchRequestWait(asyncRequest);

    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // NOT FINISHED!!!
        // Render stuff
        for (const auto& pair : *resourceManager.GetLoadedTextures())
        {
            const std::string&        resourceName = pair.first;
            std::shared_ptr<ITexture> resource     = pair.second;


            // Check if it's a Mesh
            if (std::dynamic_pointer_cast<IMesh>(resource))
            {
                std::shared_ptr<IMesh> mesh = std::dynamic_pointer_cast<IMesh>(resource);


                // e.g., mesh->LoadResource(...), mesh->UnloadResource(), etc.
            }
            // Check if it's a Texture
            else if (std::dynamic_pointer_cast<Texture>(resource))
            {
                std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(resource);
            }
        }

        // InputUpdate();
    }

    //- De-Initialization
    // RaylibImGui::Deinit();
    ImguiDeInit();
    CloseWindow();  // Close window and OpenGL context
}
