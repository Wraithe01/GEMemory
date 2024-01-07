// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "Settings.h"
#include "project.h"
#include "Renderer.h"
#include <set>
#include <format>


constexpr auto HEIGHT = 1920;
constexpr auto WIDTH  = 1080;

std::set<ImAllocator*> g_imAlloc;
StackAlloc             g_frameStack;
PoolAlloc              g_poolAlloc;

static ImAllocator* ImRegisterAllocator(Allocator* allocator);
static void         ImUnregisterAllocator(ImAllocator* allocator);
static void         ImGuiResourceTrace(void);
static void         ImGuiMemoryTrace(void);

void Run()
{
    //- Initialization
    RendererInit(HEIGHT, WIDTH);  // This must be working before imgui can work
    ImguiInit();

    ImAllocator* fs = ImRegisterAllocator(&g_frameStack);
    ImAllocator* pa = ImRegisterAllocator(&g_poolAlloc);

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
        g_frameStack.Alloc(rand() % 100);
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

        // g_frameStack.Flush();
        EndMode3D();

        DrawText("Press ESC to exit", 50, 50, 50, BLUE);

        EndDrawing();
    }

    // TODO
    // UNLOAD TEXTURES etc

    //- De-Initialization
    // RaylibImGui::Deinit();
    ImUnregisterAllocator(fs);
    ImUnregisterAllocator(pa);
    ImguiDeInit();
    CloseWindow();  // Close window and OpenGL context
}

static ImAllocator* ImRegisterAllocator(Allocator* allocator)
{
    ImAllocator* imalloc = new ImAllocator;
    imalloc->allocator   = allocator;
    g_imAlloc.insert(imalloc);
    return imalloc;
}
static void ImUnregisterAllocator(ImAllocator* allocator)
{
    g_imAlloc.erase(allocator);
    delete allocator;
    allocator = nullptr;
}


static void ImGuiResourceTrace(void)
{
    ResourceManager&       rm   = ResourceManager::GetInstance();
    static float           time = 0;
    static float           span = 30.0f;
    static ScrollingBuffer meshBuffer;
    static ScrollingBuffer texBuffer;

    time += ImGui::GetIO().DeltaTime;
    meshBuffer.AddPoint(time, rm.GetLoadedMeshes()->size());
    texBuffer.AddPoint(time, rm.GetLoadedTextures()->size());


    if (ImGui::Begin(IMGUI_RESOURCE_USAGE))
    {
        ImGui::SliderFloat("Resource Timespan", &span, 1, 30, "%.1f sec");
        const ImVec2 winsize = ImGui::GetWindowSize();

        ImGui::Text("Mesh Resource Usages (%d)", rm.GetLoadedMeshes()->size());
        if (ImGui::BeginChild("meshIds",
                              ImVec2(winsize.x / 2, winsize.y / 2),
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
        ImGui::SameLine(0, 12);
        if (ImPlot::BeginPlot("Mesh Graphical Usage", ImVec2(0, winsize.y / 2)))
        {
            static uint32_t maxMeshes = 0;
            maxMeshes                 = max(maxMeshes, rm.GetLoadedMeshes()->size());
            ImPlot::SetupAxes(
                nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxisLimits(ImAxis_X1, time - span, time, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, maxMeshes + 1, ImGuiCond_::ImGuiCond_Always);

            ImPlot::PlotLine("Loaded Meshes",
                             &meshBuffer.Data[0].x,
                             &meshBuffer.Data[0].y,
                             meshBuffer.Data.size(),
                             0,
                             meshBuffer.Offset,
                             sizeof(ImVec2));
            ImPlot::EndPlot();
        }


        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Texture Resource Usage (%d)", rm.GetLoadedTextures()->size());
        if (ImGui::BeginChild("textureIds",
                              ImVec2(winsize.x / 2, winsize.y / 2),
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
        ImGui::SameLine(0, 12);
        if (ImPlot::BeginPlot("Texture Graphical Usage", ImVec2(0, winsize.y / 2)))
        {
            static uint32_t maxTextures = 0;
            maxTextures                 = max(maxTextures, rm.GetLoadedTextures()->size());
            ImPlot::SetupAxes(
                nullptr, nullptr, ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_NoTickLabels);
            ImPlot::SetupAxisLimits(ImAxis_X1, time - span, time, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, maxTextures + 1, ImGuiCond_::ImGuiCond_Always);

            ImPlot::PlotLine("Loaded Textures",
                             &texBuffer.Data[0].x,
                             &texBuffer.Data[0].y,
                             meshBuffer.Data.size(),
                             0,
                             meshBuffer.Offset,
                             sizeof(ImVec2));
            ImPlot::EndPlot();
        }
    }
    ImGui::End();
}
static void ImGuiMemoryTrace(void)
{
    static float span = 30.0f;
    static float time = 0;
    time += ImGui::GetIO().DeltaTime;

    if (ImGui::Begin(IMGUI_MEMORY_USAGE, NULL, ImGuiWindowFlags_AlwaysVerticalScrollbar))
    {
        ImGui::SliderFloat("Memory Timespan", &span, 1, 30, "%.1f sec");
        ImVec2 winSize = ImGui::GetWindowSize();

        ImGui::Text("Registered Allocators: %d", g_imAlloc.size());
        ImGui::Separator();
        ImGui::Spacing();

        uint16_t index = 0;
        for (const auto& ImAlloc : g_imAlloc)
        {
            // Add allocator data
            float usagepercent = (float) (*ImAlloc).allocator->CurrentStored()
                                 / (*ImAlloc).allocator->GetCapacity() * 100.0f;
            (*ImAlloc).buffer.AddPoint(time, usagepercent);

            if (ImGui::BeginChild(std::format("alloc {}", index).c_str(),
                                  ImVec2(-1, winSize.y / 2)))
            {
                ImGui::Text("Capacity used %d / %d (%0.2f%%)",
                            (*ImAlloc).allocator->CurrentStored(),
                            (*ImAlloc).allocator->GetCapacity(),
                            usagepercent);

                if (ImPlot::BeginPlot(std::format("[{}] {} Allocator's Graph.",
                                                  index++,
                                                  (*ImAlloc).allocator->GetAllocName())
                                          .c_str(),
                                      ImVec2(-1, 200)))
                {
                    ImPlot::SetupAxes(nullptr,
                                      nullptr,
                                      ImPlotAxisFlags_NoTickLabels,
                                      ImPlotAxisFlags_NoTickLabels);
                    ImPlot::SetupAxisLimits(ImAxis_X1, time - span, time, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 101, ImGuiCond_Always);

                    ImPlot::PlotLine("Usage (%)",
                                     &(*ImAlloc).buffer.Data[0].x,
                                     &(*ImAlloc).buffer.Data[0].y,
                                     (*ImAlloc).buffer.Data.size(),
                                     0,
                                     (*ImAlloc).buffer.Offset,
                                     sizeof(ImVec2));
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
