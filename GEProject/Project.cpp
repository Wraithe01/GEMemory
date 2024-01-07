// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "Settings.h"
#include "project.h"
#include "Renderer.h"
#include <set>
#include <format>

constexpr auto HEIGHT = 1920;
constexpr auto WIDTH  = 1080;

std::set<Allocator*> g_imAlloc;
StackAlloc           g_frameStack;
PoolAlloc            g_poolAlloc;

void InitAllocators(void)
{
    g_imAlloc.insert(&g_frameStack);
    g_imAlloc.insert(&g_poolAlloc);
}
void DeinitAllocators(void)
{
    g_imAlloc.erase(&g_frameStack);
    g_imAlloc.erase(&g_poolAlloc);
}

static void ImGuiResourceTrace(void);
static void ImGuiMemoryTrace(void);

void Run()
{
    //- Initialization
    RendererInit(HEIGHT, WIDTH);  // This must be working before imgui can work
    ImguiInit();
    InitAllocators();


    // Basic scene
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    Scene            scene;
    // scene.AppendGUID("7023d2a2-6f6d-49fc-9512-c1a3bc78b278"); // PNG
    // scene.AppendGUID("798cb1b8-dfb9-4453-b2f7-ae67f75556e"); //JPG
    // scene.AppendGUID("8baa3bc4-1b74-4015-971c-a91073603842"); //STL
    // scene.AppendGUID("d68241af-c97f-4be3-948a-9b427413ab5"); // FBX cactus
    scene.AppendGUID("bf2e0941-ad77-4d57-bd74-b7d43dc828ff");

    // Load scene
    auto asyncRequest = resourceManager.LoadScene(scene);
    resourceManager.AsynchRequestWait(asyncRequest);

    // Temp default material
    Material matDefault                         = LoadMaterialDefault();
    matDefault.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

    // Temp default position of the objects
    Matrix transform = MatrixTranslate(0.0f, 0.0f, 0.0f);

    // For some reason it does not work if we upload the mesh in the Resource class after we convert
    // it to raylib mesh. Not sure why...
    for (const auto& pair : *resourceManager.GetLoadedMeshes())
    {
        const std::string& resourceName = pair.first;
        IMesh*             resource     = pair.second.get();

        int count = resource->GetMeshCount();

        for (int i = 0; i < count; i++)
        {
            UploadMesh(&resource->GetMeshes()[i], false);
        }
    }

    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        g_frameStack.Alloc(1);
        // Movement Update
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        for (const auto& pair : *resourceManager.GetLoadedMeshes())
        {
            const std::string& resourceName = pair.first;
            IMesh*             resource     = pair.second.get();
            int                count        = resource->GetMeshCount();

            for (int i = 0; i < count; i++)
            {
                const Mesh& mesh = resource->GetMeshes()[i];
                DrawMesh(mesh, matDefault, transform);
            }
        }
        // Grid floor
        DrawGrid(100, 1.0);

        ImguiBegin();
        ImGuiResourceTrace();
        ImGuiMemoryTrace();
        ImguiEnd();

        g_frameStack.Flush();
        EndMode3D();

        DrawText("Press ESC to exit", 50, 50, 50, BLUE);

        EndDrawing();
    }

    // TODO
    // UNLOAD TEXTURES etc

    //- De-Initialization
    // RaylibImGui::Deinit();
    DeinitAllocators();
    ImguiDeInit();
    CloseWindow();  // Close window and OpenGL context
}


static void ImGuiResourceTrace(void)
{
    ResourceManager& rm = ResourceManager::GetInstance();

    if (ImGui::Begin(IMGUI_RESOURCE_USAGE))
    {
        const ImVec2 winsize = ImGui::GetWindowSize();

        ImGui::Text("Mesh Resource Usages (%d)", rm.GetLoadedMeshes()->size());
        if (ImGui::BeginChild("meshIds",
                              ImVec2(winsize.x / 2, winsize.y / 3),
                              true,
                              ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            uint16_t index = 0;
            for (const auto& mesh : *rm.GetLoadedMeshes())
            {
                ImGui::Text("[%d]: %s", index++, mesh.first.c_str());
                ImGui::Separator();
                ImGui::Spacing();
            }
        }
        ImGui::EndChild();
        ImGui::SameLine(0, 6);
        ImGui::Text("Graph Meshes");


        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Texture Resource Usage (%d)", rm.GetLoadedTextures()->size());
        if (ImGui::BeginChild("textureIds",
                              ImVec2(winsize.x / 2, winsize.y / 3),
                              true,
                              ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            uint16_t index = 0;
            for (const auto& tex : *rm.GetLoadedTextures())
            {
                ImGui::Text("[%d]: %s", index++, tex.first.c_str());
                ImGui::Separator();
                ImGui::Spacing();
            }
        }
        ImGui::EndChild();
        ImGui::SameLine(0, 6);
        ImGui::Text("Graph Textures");
    }
    ImGui::End();
}
static void ImGuiMemoryTrace(void)
{
    if (ImGui::Begin(IMGUI_MEMORY_USAGE, NULL, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        ImVec2 winSize = ImGui::GetWindowSize();
        ImGui::Text("Active Allocators: %d", g_imAlloc.size());
        ImGui::Separator();
        ImGui::Spacing();

        uint16_t index = 0;
        for (const auto& allocator : g_imAlloc)
        {
            if (ImGui::BeginChild(std::format("alloc {}", index).c_str(),
                                  ImVec2(winSize.x / 2, winSize.y / 2)))
            {
                ImGui::Text("Capacity used %d", allocator->CurrentStored());

                if (ImPlot::BeginPlot(std::format("[{}] {} Allocator's Graph.",
                                                  index++,
                                                  allocator->GetAllocName())
                                          .c_str(),
                                      ImVec2(winSize.x / 2, winSize.y / 3)))
                {
                    uint32_t data[] = { 0, 0, 1, 2, 0, 0, 0, 1, 0, 0, 1, 0, 1,
                                        2, 3, 4, 5, 4, 3, 2, 1, 0, 0, 0, 0 };
                    ImPlot::PlotLine("line", data, sizeof(data) / sizeof(*data));
                    ImPlot::EndPlot();
                }
            }
            ImGui::EndChild();
            ImGui::Separator();
            ImGui::Spacing();
        }
    }
    ImGui::End();
}
