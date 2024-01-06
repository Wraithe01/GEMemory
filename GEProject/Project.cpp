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

    std::vector<Model> models;

    //model.materials[0].shader = shader;
    //cube.materials[0].shader = shader;

    int ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    SetShaderValue(shader, ambientLoc, &ambient, SHADER_UNIFORM_VEC4);
   
    Light lights[MAX_LIGHTS] = { 0 };
    Vector3 lightPos1 = { -2, 1, -2 };
    Vector3 lightPos2 = { 2, 1, 2 };
    Vector3 lightPos3 = { -2, 1, 2 };
    Vector3 lightPos4 = { 2, 1, -2 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, lightPos1, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_DIRECTIONAL, lightPos2, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_DIRECTIONAL, lightPos3, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_DIRECTIONAL, lightPos4, Vector3Zero(), BLUE, shader);

    // Basic scene
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    Scene scene;
    scene.AppendScene("scene1");
    scene.AppendScene("scene2");
    scene.AppendScene("scene3");
    scene.AppendScene("scene4");
    scene.AppendScene("scene5");
    scene.AppendScene("scene6");

    // Load scene
    auto asyncRequest = resourceManager.LoadScene(scene);
    resourceManager.AsynchRequestWait(asyncRequest);

    // Temp default material
    //Material matDefault = LoadMaterialDefault();
    //matDefault.maps[MATERIAL_MAP_DIFFUSE].color = GREEN;

    // Temp default position of the objects
    Matrix transform = MatrixTranslate(0.0f, 0.0f, 0.0f);
    Matrix rotation = MatrixRotateX(DEG2RAD * 270.0f);

    // For some reason it does not work if we upload the mesh in the Resource class after we convert it to raylib mesh. Not sure why...
    for (const auto& pair : *resourceManager.GetLoadedMeshes())
    {
        const std::string& resourceName = pair.first;
        IMesh* resource = pair.second.get();

        int count = resource->GetMeshCount();
        
        for (int i = 0; i < count; i++)
        {
            UploadMesh(&resource->GetMeshes()[i], false);

            Model model = LoadModelFromMesh(resource->GetMeshes()[i]);
            model.materials[0].shader = shader;
            model.transform = MatrixMultiply(model.transform, rotation);
            models.push_back(model);
        }
    }


    //- Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Movement Update
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Update light values (actually, only enable/disable them)
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(camera);

        for (int i = 0; i < models.size(); i++)
        {
            DrawModel(models[i], Vector3Zero(), 1.0f, WHITE);
        }

        // Draw spheres to show where the lights are
        for (int i = 0; i < MAX_LIGHTS; i++)
        {
            if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
            else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        }

        for (const auto& pair : *resourceManager.GetLoadedMeshes())
        {
            const std::string& resourceName = pair.first;
            IMesh* resource = pair.second.get();
            int count = resource->GetMeshCount();

            for (int i = 0; i < count; i++)
            {
                const Mesh& mesh = resource->GetMeshes()[i];
                //DrawMesh(mesh, matDefault, transform);
            }
        }
        // Grid floor
        DrawGrid(100, 1.0);

        //DisableLighting();
        EndMode3D();

        DrawText("Press ESC to exit", 50, 50, 50, BLUE);

        EndDrawing();
    }

    // TODO
    // UNLOAD TEXTURES etc

    UnloadShader(shader);   // Unload shader

    //- De-Initialization
    //RaylibImGui::Deinit();
    ImguiDeInit();
    CloseWindow();  // Close window and OpenGL context
}
