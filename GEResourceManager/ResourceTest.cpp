#include "ResourceTest.h"

using namespace std::chrono;

ResourceTest::ResourceTest() {}

ResourceTest::~ResourceTest() {}

void ResourceTest::Validate() {
	ResourceManager& resourceManager = ResourceManager::GetInstance();

	std::cout << "[+] Starting validation test!\n";
	std::cout << "TEST 1 - BASIC SCENE LOAD OF 12 RESOURCES (SHOULD NOT CAUSE ERRORS)\n";
	std::cout << "========================= Start =========================\n";
	Scene scene;
	scene.AppendGUID("7023d2a2-6f6d-49fc-9512-c1a3bc78b278");
	scene.AppendGUID("798cb1b8-dfb9-4453-b2f7-ae67f75556e");
	scene.AppendGUID("8baa3bc4-1b74-4015-971c-a91073603842");
	scene.AppendGUID("bf2e0941-ad77-4d57-bd74-b7d43dc828ff");

	// Load scene
	auto asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	printf("Scene loaded! Current resources: %zu\n", resourceManager.GetNumOfLoadedRes());

	// Unload scene
	resourceManager.UnloadScene(scene);
	std::cout << "========================= End =========================\n\n";

	std::cout << "TEST 2 - MEMORY LIMIT TEST (SHOULD CAUSE ERRORS)\n";
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

	std::cout << "TEST 3 - SCENE MEMORY TEST (SHOULD NOT CAUSE ERRORS)\n";
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


	std::cout << "TEST 4 - ASYNC TEST (SHOULD NOT LOCK MAIN THREAD)\n";
	std::cout << "========================= Start =========================\n";
	asyncRequest = resourceManager.LoadScene(scene);

	while (true)
	{
		if (!resourceManager.AsynchRequestCompleted(asyncRequest))
		{
			printf("Main thread not locked! We sleep 100 ms to wait for the request to complete...\n");
			Sleep(100);
		}
		else {
			printf("Work complete!\n");
			break;
		}
	}
	resourceManager.UnloadScene(scene);
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
	printf("Guids: %zu\n", scene.GetChunk().size());
	std::cout << "SUBTEST 1. Load all resources and unload once\n";
	auto tstart = high_resolution_clock::now();
	auto asyncRequest = resourceManager.LoadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	
	asyncRequest = resourceManager.UnloadScene(scene);
	resourceManager.AsynchRequestWait(asyncRequest);
	std::printf("Benchmark test took %8.8lld micro seconds\n",
		duration_cast<microseconds>(high_resolution_clock::now() - tstart).count());

	std::cout << "SUBTEST 2. Load all resources and unload 10 times\n";
	tstart = high_resolution_clock::now();

	for (size_t i = 0; i < 10; i++)
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