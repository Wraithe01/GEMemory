// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "project.h"
#include "Renderer.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

constexpr auto HEIGHT = 1920;
constexpr auto WIDTH = 1080;

#define ActivateChunk(chunk)                 \
if (!activeChunk[chunk])                     \
{                                            \
    activeChunk[chunk] = true;               \
    resourceManager.LoadScene(scenes[chunk]);\
}

#define DeactivateChunk(chunk)                 \
if (activeChunk[chunk])                        \
{                                              \
    activeChunk[chunk] = false;                \
    resourceManager.UnloadScene(scenes[chunk]);\
}

void Run()
{
    //- Initialization
    RendererInit(HEIGHT, WIDTH);  // This must be working before imgui can work
    ImguiInit();

    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat("../shaders/lighting.vs"), TextFormat("../shaders/lighting.fs"));

    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);

    Light lights[MAX_LIGHTS] = { 0 };
    Vector3 pos = { 0, 1, 0 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, pos, { 1, 0, 0 }, RED, shader);
    lights[1] = CreateLight(LIGHT_DIRECTIONAL, pos, { 0, 0, 1 }, GREEN, shader);
    lights[2] = CreateLight(LIGHT_DIRECTIONAL, pos, { -1, 0, 0 }, BLUE, shader);
    lights[3] = CreateLight(LIGHT_DIRECTIONAL, pos, { 0, 1, -1 }, YELLOW, shader);

    // Default scenes
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    Scene scenes[6] = {};
    scenes[0].AppendChunk("chunk1");
    scenes[1].AppendChunk("chunk2");
    scenes[2].AppendChunk("chunk3");
    scenes[3].AppendChunk("chunk4");
    scenes[4].AppendChunk("chunk5");
    scenes[5].AppendChunk("chunk6");
    std::string pngTexture = "702e3416-3cc1-4ae5-8f7e-1a439bc2951f";
    scenes[0].AppendGUID(pngTexture); //PNG for texture in scene1 (fbx scene, DJ board people)
    
    // ugly workaround for making texture work
    auto asyncRequest = resourceManager.LoadScene(scenes[0]);
    resourceManager.AsynchRequestWait(asyncRequest);

    ITexture* myTexture = resourceManager.GetTexture(pngTexture).get();
    int size = myTexture->GetWidth() * myTexture->GetHeight() * myTexture->GetChannels();

    Image image{ 0 };
    image.width = myTexture->GetWidth();
    image.height = myTexture->GetHeight();
    image.data = (void*)myTexture->GetImage();
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
    image.mipmaps = 1;

    Texture2D texture = LoadTextureFromImage(image);

    // Default material
    Material matDefault = LoadMaterialDefault();
    matDefault.maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    matDefault.shader = shader;

    // Default transform
    Matrix rotation = MatrixRotateX(DEG2RAD * 270.0f);
    Matrix scale = MatrixScale(1.5f, 1.5f, 1.5f);
    Matrix transform = MatrixMultiply(scale, rotation);

    camera.position = { 45, 3, 0 };

    // chunk logic
    bool activeChunk[6] = { true, false, false, false, false, false };
    float angle = 0;
    Vector2 angleOrig = { 0, 1 };

    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        angle = -Vector2Angle(angleOrig, { camera.position.x, camera.position.z }) + PI;

        if (((angle >= 0) && (angle < PI / 3 - PI / 6)) || ((angle >= 2*PI - PI / 6) && (angle <= 2*PI)))
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
        else if ((angle >= 2 * PI / 3 - PI / 6) && (angle < PI - PI/6))
        {
            ActivateChunk(0);
            ActivateChunk(1);
            DeactivateChunk(2);
            DeactivateChunk(3);
            DeactivateChunk(4);
            ActivateChunk(5);
        }
        else if ((angle >= PI - PI/6) && (angle < 4 * PI / 3))
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
        else if ((angle >= 5 * PI / 3 - PI / 6) && (angle <= 2*PI - PI / 6))
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
        for (int i = 0; i < MAX_LIGHTS; i++) {
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
            IMesh* resource = pair.second.get();
            size_t count = resource->GetMeshCount();

            if (!resource->Uploaded) continue;

            for (size_t i = 0; i < count; i++)
            {
                DrawMesh(resource->GetMeshes()[i], matDefault, transform);
            }
        }
        resourceManager.loadedUnlock();

        // Grid floor
        DrawGrid(100, 1.0);

        EndMode3D();

        DrawText("Press ESC to exit", 50, 50, 50, BLUE);

        EndDrawing();
    }

    //- De-Initialization
    UnloadShader(shader);   // Unload shader
    UnloadTexture(texture); // Unload default texture

    //RaylibImGui::Deinit();
    ImguiDeInit();
    CloseWindow();  // Close window and OpenGL context
}
