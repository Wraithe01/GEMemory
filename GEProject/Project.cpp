// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "Settings.h"
#include "project.h"
#include "Renderer.h"
#include <set>
#include <format>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

constexpr auto HEIGHT = 1920;
constexpr auto WIDTH  = 1080;

std::set<ImAllocator*> g_imAlloc;
static ImAllocator* ImRegisterAllocator(Allocator* allocator);
static void         ImUnregisterAllocator(ImAllocator* allocator);
static void         ImGuiResourceTrace(void);
static void         ImGuiMemoryTrace(void);

#define ActivateChunk(chunk)                 \
if (!activeChunk[chunk])                     \
{                                            \
    activeChunk[chunk] = true;               \
    resourceManager.LoadScene(scenes[chunk]);\
}

#define DeactivateChunk(chunk)                                                                     \
    if (activeChunk[chunk])                                                                        \
    {                                                                                              \
        activeChunk[chunk] = false;                                                                \
        resourceManager.UnloadScene(scenes[chunk]);                                                \
    }


void Run()
{
    ResourceManager& resourceManager = ResourceManager::GetInstance();

    //- Initialization
    RendererInit(HEIGHT, WIDTH);  // This must be working before imgui can work
    ImguiInit();

    ImAllocator* stackp = ImRegisterAllocator(resourceManager.GetStack());

    // Load basic lighting shader
    Shader shader
        = LoadShader(TextFormat("../shaders/lighting.vs"), TextFormat("../shaders/lighting.fs"));

    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int   ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    Light   lights[MAX_LIGHTS] = { 0 };
    Vector3 pos                = { 0, 1, 0 };
    lights[0]                  = CreateLight(LIGHT_DIRECTIONAL, pos, { 1, 0, 0 }, RED, shader);
    lights[1]                  = CreateLight(LIGHT_DIRECTIONAL, pos, { 0, 0, 1 }, GREEN, shader);
    lights[2]                  = CreateLight(LIGHT_DIRECTIONAL, pos, { -1, 0, 0 }, BLUE, shader);
    lights[3]                  = CreateLight(LIGHT_DIRECTIONAL, pos, { 0, 1, -1 }, YELLOW, shader);

    // Default scenes
    Scene scenes[6] = {};
    scenes[0].AppendChunk("chunk1");
    scenes[1].AppendChunk("chunk2");
    scenes[2].AppendChunk("chunk3");
    scenes[3].AppendChunk("chunk4");
    scenes[4].AppendChunk("chunk5");
    scenes[5].AppendChunk("chunk6");
    std::string pngTexture = "702e3416-3cc1-4ae5-8f7e-1a439bc2951f";
    scenes[0].AppendGUID(pngTexture);  // PNG for texture in scene1 (fbx scene, DJ board people)

    // ugly workaround for making texture work
    auto asyncRequest = resourceManager.LoadScene(scenes[0]);
    resourceManager.AsynchRequestWait(asyncRequest);

    ITexture* myTexture = resourceManager.GetTexture(pngTexture).get();
    int       size      = myTexture->GetWidth() * myTexture->GetHeight() * myTexture->GetChannels();

    Image image{ 0 };
    image.width   = myTexture->GetWidth();
    image.height  = myTexture->GetHeight();
    image.data    = (void*) myTexture->GetImage();
    image.format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    image.mipmaps = 1;

    Texture2D texture = LoadTextureFromImage(image);

    // Default material
    Material matDefault                           = LoadMaterialDefault();
    matDefault.maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    matDefault.shader                             = shader;

    // Default transform
    Matrix rotation  = MatrixRotateX(DEG2RAD * 270.0f);
    Matrix scale     = MatrixScale(1.5f, 1.5f, 1.5f);
    Matrix transform = MatrixMultiply(scale, rotation);

    camera.position = { 45, 3, 0 };

    // chunk logic
    bool    activeChunk[6] = { true, false, false, false, false, false };
    float   angle          = 0;
    Vector2 angleOrig      = { 0, 1 };

    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        angle = -Vector2Angle(angleOrig, { camera.position.x, camera.position.z }) + PI;

        if (((angle >= 0) && (angle < PI / 3 - PI / 6))
            || ((angle >= 2 * PI - PI / 6) && (angle <= 2 * PI)))
        {
            DeactivateChunk(0);
            DeactivateChunk(1);
            DeactivateChunk(2);
            ActivateChunk(3);
            ActivateChunk(4);
            ActivateChunk(5);
        }
        else if ((angle >= PI / 3 - PI / 6) && (angle < 2 * PI / 3 - PI / 6))
        {
            ActivateChunk(0);
            DeactivateChunk(1);
            DeactivateChunk(2);
            DeactivateChunk(3);
            ActivateChunk(4);
            ActivateChunk(5);
        }
        else if ((angle >= 2 * PI / 3 - PI / 6) && (angle < PI - PI / 6))
        {
            ActivateChunk(0);
            ActivateChunk(1);
            DeactivateChunk(2);
            DeactivateChunk(3);
            DeactivateChunk(4);
            ActivateChunk(5);
        }
        else if ((angle >= PI - PI / 6) && (angle < 4 * PI / 3))
        {
            ActivateChunk(0);
            ActivateChunk(1);
            ActivateChunk(2);
            DeactivateChunk(3);
            DeactivateChunk(4);
            DeactivateChunk(5);
        }
        else if ((angle >= 4 * PI / 3 - PI / 6) && (angle < 5 * PI / 3 - PI / 6))
        {
            DeactivateChunk(0);
            ActivateChunk(1);
            ActivateChunk(2);
            ActivateChunk(3);
            DeactivateChunk(4);
            DeactivateChunk(5);
        }
        else if ((angle >= 5 * PI / 3 - PI / 6) && (angle <= 2 * PI - PI / 6))
        {
            DeactivateChunk(0);
            DeactivateChunk(1);
            ActivateChunk(2);
            ActivateChunk(3);
            ActivateChunk(4);
            DeactivateChunk(5);
        }

        // Movement Update
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Update light values
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            UpdateLightValues(shader, lights[i]);
        }

        // Uploaded new meshes to GPU if needed
        resourceManager.UploadQueuedMeshes();

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        resourceManager.LoadedLock();
        for (const auto& pair : *resourceManager.GetLoadedMeshes())
        {
            const std::string& resourceName = pair.first;
            IMesh*             resource     = pair.second.get();
            size_t             count        = resource->GetMeshCount();

            if (!resource->Uploaded)
                continue;

            for (size_t i = 0; i < count; i++)
            {
                DrawMesh(resource->GetMeshes()[i], matDefault, transform);
            }
        }
        resourceManager.loadedUnlock();

        // Grid floor
        DrawGrid(100, 1.0);

        ImguiBegin();
        ImGuiResourceTrace();
        ImGuiMemoryTrace();
        ImguiEnd();

        ((StackAlloc*) (resourceManager.GetStack()))->Flush();

        EndMode3D();

        DrawText("Press ESC to exit", 50, 50, 50, BLUE);

        EndDrawing();
    }

    //- De-Initialization
    UnloadShader(shader);    // Unload shader
    UnloadTexture(texture);  // Unload default texture

    ImUnregisterAllocator(stackp);

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
            ImPlot::SetupAxisLimits(ImAxis_Y1, -1, maxMeshes * 1.1f, ImGuiCond_::ImGuiCond_Always);

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
            ImPlot::SetupAxisLimits(
                ImAxis_Y1, -1, maxTextures * 1.1f, ImGuiCond_::ImGuiCond_Always);

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
