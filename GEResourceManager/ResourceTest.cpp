#include "ResourceTest.h"

using namespace std::chrono;

ResourceTest::ResourceTest() {}

ResourceTest::~ResourceTest() {}

void ResourceTest::Validate() {
	ResourceManager& resourceManager = ResourceManager::GetInstance();

	std::cout << "[+] Starting validation test!\n";
	std::cout << "TEST 1 - SHOULD NOT CAUSE ERRORS\n";
	std::cout << "========================= Start =========================\n";
	Scene scene;
	scene.AppendGUID("6e4fef1c-c32-4601-adab-42c61ebc365d");

	// Load scene
	auto asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	printf("Scene loaded! Current resources: %zu\n", resourceManager.GetNumOfLoadedRes());

	// Unload scene
	resourceManager.UnloadScene(scene);
	std::cout << "========================= End =========================\n\n";

	std::cout << "TEST 2 - SHOULD CAUSE ERRORS\n";
	std::cout << "========================= Start =========================\n";

	// We update the memory limit to 0.5MB
	resourceManager.SetMemoryLimit(500000);

	asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	printf("Scene loaded! Current resources: %zu\n", resourceManager.GetNumOfLoadedRes());
	resourceManager.UnloadScene(scene);
	// We restore default memory limit
	resourceManager.SetMemoryLimit(DEFAULT_MEMORY_LIMIT);
	std::cout << "========================= End =========================\n\n";

	std::cout << "TEST 3 - SCENE MEMORY TEST\n";
	std::cout << "========================= Start =========================\n";

	asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	size_t initMemory = resourceManager.GetMemoryUsage();
	Scene scene2;
	scene2 = scene;
	asyncRequest = resourceManager.LoadScene(scene2);
	resourceManager.AsynchRequestWait(asyncRequest);
	size_t currentMemory = resourceManager.GetMemoryUsage();

	if (initMemory == resourceManager.GetMemoryUsage())
	{
		printf("PASS! Before: %zu bytes, After: %zu bytes\n", initMemory, currentMemory);
	}
	else
	{
		printf("FAIL! Memory changed! Before: %zu bytes, After: %zu bytes\n", initMemory, currentMemory);
	}
	resourceManager.UnloadScene(scene);
	resourceManager.UnloadScene(scene2);
	std::cout << "========================= End =========================\n\n";
}

void ResourceTest::PerformanceBenchmark() {
	ResourceManager& resourceManager = ResourceManager::GetInstance();

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
	auto asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	
	asyncRequest = resourceManager.UnloadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	std::printf("Benchmark test took %8.8lld micro seconds\n",
		duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());

	std::cout << "SUBTEST 2. Load all resources and unload 10 000 times\n";
	tstart = high_resolution_clock::now();

	for (size_t i = 0; i < 10000; i++)
	{
		asyncRequest = resourceManager.LoadScene(scene);
		resourceManager.AsynchRequestWait(asyncRequest);
		asyncRequest = resourceManager.UnloadScene(scene);
		resourceManager.AsynchRequestWait(asyncRequest);
	}
	std::printf("Benchmark test took %8.8lld micro seconds\n",
		duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());
	std::cout << "========================= End =========================\n\n";

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
	// TODO 
}