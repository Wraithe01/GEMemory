// This is the main folder where "Run" is the entrypoint of the program

#include "Includes.h"
#include "project.h"
#include "Renderer.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

constexpr auto HEIGHT = 1920;
constexpr auto WIDTH = 1080;

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

    resourceManager.UnloadScene(scenes[0]);

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

    camera.position = { /*45*/1, 3, 0 };

    // chunk logic
    bool activeChunk[6] = { false };
    float angle = 0;
    Vector2 angleOrig = { 0, -1 };

    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        angle = -Vector2Angle(angleOrig, { camera.position.x, camera.position.z }) + PI;
        std::cout << angle << std::endl;

        if ((angle >= 0) && (angle < PI / 3))
        {
            std::cout << "chunk 1\n";
        }
        else if ((angle >= PI / 3) && (angle < 2 * PI / 3))
        {
            if (!activeChunk[0])
            {
                activeChunk[0] = true;
                resourceManager.LoadScene(scenes[0]);
            }
        }
        else if ((angle >= 2 * PI / 3) && (angle < PI))
        {
            if (!activeChunk[1])
            {
                activeChunk[1] = true;
                resourceManager.LoadScene(scenes[1]);
            }
        }
        else if ((angle >= PI) && (angle < 4 * PI / 3))
        {
            if (!activeChunk[2])
            {
                activeChunk[2] = true;
                resourceManager.LoadScene(scenes[1]);
            }
        }
        else if ((angle >= 4 * PI / 3) && (angle < 5 * PI / 3))
        {
            std::cout << "chunk 5\n";
        }
        else if ((angle >= 5 * PI / 3) && (angle <= 2 * PI))
        {
            std::cout << "chunk 6\n";
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

            for (size_t i = 0; i < count; i++)
            {
                //DrawMesh(resource->GetMeshes()[i], matDefault, transform);
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
