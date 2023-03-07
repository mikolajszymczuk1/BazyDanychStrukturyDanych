#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <mutex>

std::mutex mtx;
std::atomic<bool> found = false;
std::vector<long long> dividers;

// Function that find all dividers
void findDividers(long long number, long long start, long long end);

// Main program
int main() {
  auto start = std::chrono::high_resolution_clock::now();

  long long n = 12534565234;
  std::vector<std::thread> threads;
  int threadsCount = std::thread::hardware_concurrency();
  std::cout << "Number of hardware threads: " << threadsCount << std::endl;

  // Calculate chunk size for each thread
  long long chunkSize = (n + threadsCount - 1) / threadsCount;
  std::cout << "Single chunk size: " << chunkSize << std::endl;

  // Create threads
  for (int i = 0; i < threadsCount; i++) {
    long long start = i * chunkSize + 1;
    long long end = std::min((i + 1) * chunkSize, n);
    threads.push_back(std::thread(findDividers, n, start, end));
    std::unique_lock<std::mutex> lock(mtx);
  }

  // Wait for all threads
  for (auto &th : threads) {
    th.join();
  }

  std::cout << "Success !" << std::endl;

  // Calculate program running time
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
  std::cout << "Total time: " << duration.count() << "s" << std::endl;

  return 0;
}

// ====== Functions implementations ======

void findDividers(long long number, long long start, long long end) {
  for (unsigned long long i = start; i < end && !found; i++) {
    if (number % i == 0) {
      std::lock_guard<std::mutex> guard(mtx);
      dividers.push_back(i);

      if (number / i != i) {
        dividers.push_back(number / i);
      }

      if (dividers.size() >= 100) {
        found = true;
      }
    }
  }
}
