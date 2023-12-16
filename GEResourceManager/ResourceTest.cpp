#include "ResourceTest.h"

using namespace std::chrono;

ResourceTest::ResourceTest() {}

ResourceTest::~ResourceTest() {}

void ResourceTest::Validate() {
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	std::cout << "[+] Starting validation test!\n";
	std::cout << "TEST 1 - SHOULD NOT CAUSE ERRORS\n";
	std::cout << "========================= Start =========================\n";
	Scene scene;
	scene.AppendGUID("2782cc3f-194c-436f-93f1-502e14227a39");
	scene.AppendGUID("28d7d94-135a-413b-bdfd-95519a9d2f8");
	scene.AppendGUID("539b33cb-af8d-4287-81a2-7e87eec372d");
	scene.AppendGUID("65d5de75-9365-4b2c-b7dc-d7dfaf5636cc");
	scene.AppendGUID("913b5a67-ec17-4fe3-823d-6aada1ce994");
	scene.AppendGUID("97c2867a-5879-4cb9-917d-f1b18106ed3");
	scene.AppendGUID("a965f7fd-db8e-4efb-8dbf-852abcede398");
	scene.AppendGUID("b84cad92-5370-4103-8c46-7df5d7f83e4b");
	scene.AppendGUID("be7561e3-9a23-46bf-85a3-9051746c8fd6");
	scene.AppendGUID("ea07e00f-6e79-4cc9-b648-fa559971fa2b");
	scene.AppendGUID("f2f94932-1e62-4a34-874e-7bc7a73149fc");
	scene.AppendGUID("f6ab137b-b72d-4967-adc-b8404ade3e53");
	
	// Load scene
	resourceManager->LoadScene(scene);
	std::cout << "Scene loaded!\n";

	// Unload scene
	resourceManager->UnloadScene(scene);
	std::cout << "========================= End =========================\n";

	std::cout << "TEST 2 - SHOULD CAUSE ERRORS\n";
	std::cout << "========================= Start =========================\n";

	// TODO: Update memory limit to cause errors
	// Load scene with too many objects
	resourceManager->LoadScene(scene);
	std::cout << "Scene loaded!\n";
	resourceManager->UnloadScene(scene);
	std::cout << "========================= End =========================\n";

	std::cout << "TEST 3 - SCENE MEMORY TEST\n";
	std::cout << "========================= Start =========================\n";

	// TODO
	// Load 2 scenes
	// load 1 scene, save memory, then load second scene to make sure the memory is still the same
}

void ResourceTest::PerformanceBenchmark() {
	ResourceManager* resourceManager = ResourceManager::GetInstance();

	std::cout << "[+] Starting performance benchmark test!\n";
	std::cout << "TEST 1 (1 package, 1 000 resources in total)\n";
	std::cout << "========================= Start =========================\n";
	Scene scene;

	for (size_t i = 0; i < 1000; i++)
	{
		// Generate the last four digits with leading zeros
		std::ostringstream oss;
		oss << std::setw(4) << std::setfill('0') << i;

		// Construct the full GUID with the generated suffix
		std::string guid = "00000000-0000-0000-0000-00000000" + oss.str();
		
		scene.AppendGUID(guid);
	}

	std::cout << "SUBTEST 1. Load all resources and unload once\n";
	auto tstart = high_resolution_clock::now();
	resourceManager->LoadScene(scene);
	resourceManager->UnloadScene(scene);
	std::printf("Benchmark test took %8.8lld micro seconds\n",
		duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());

	std::cout << "SUBTEST 2. Load all resources and unload 10 000 times\n";
	auto tstart = high_resolution_clock::now();

	for (size_t i = 0; i < 10000; i++)
	{
		resourceManager->LoadScene(scene);
		resourceManager->UnloadScene(scene);
	}
	std::printf("Benchmark test took %8.8lld micro seconds\n",
		duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
	std::cout << "========================= End =========================\n";

	std::cout << "TEST 2 (5 packages, 1 000 resources in total)\n";
	std::cout << "========================= Start =========================\n";
	for (size_t i = 0; i < 1000; i++)
	{
		// Generate the last four digits with leading zeros
		std::ostringstream oss;
		oss << std::setw(4) << std::setfill('0') << i;

		// Construct the full GUID with the generated suffix
		std::string guid = "00000000-0000-0000-0000-00000000" + oss.str();

		scene.AppendGUID(guid);
	}
}