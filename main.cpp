#include <chrono>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

const uint64_t num_cores = static_cast<uint64_t>(std::thread::hardware_concurrency());

void PrintHelp()
{
  std::cout << "./main.exe single 8" << std::endl << "./main.exe multi 8" << std::endl;
}

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    PrintHelp();
    return EXIT_FAILURE;
  }
  uint64_t size = 0;
  try
  {
    size = std::stoull(argv[2]);
  }
  catch (...)
  {
    PrintHelp();
    return EXIT_FAILURE;
  }
  const uint64_t byte_size = 1024ull * 1024ull * size;
  std::unique_ptr<char[]> data = std::make_unique<char[]>(byte_size);
  const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  if (std::strncmp(argv[1], "single", 6) == 0)
  {
    std::memset(data.get(), 0, byte_size);
  }
  else if (std::strncmp(argv[1], "multi", 5) == 0)
  {
    std::vector<std::thread> threads;
    const uint64_t sub_size = byte_size / num_cores;
    for (uint64_t i = 0; i < num_cores; ++i)
    {
      threads.emplace_back(std::thread([&data, offset = i * sub_size, sub_size]() { std::memset(data.get() + offset, 0, sub_size); }));
    }
    for (std::thread& thread : threads)
    {
      thread.join();
    }
  }
  else
  {
    PrintHelp();
    return EXIT_FAILURE;
  }
  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() << std::endl;
  return EXIT_SUCCESS;
}

