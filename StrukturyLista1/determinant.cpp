#include <iostream>
#include <vector>
#include <thread>

using namespace std;

// Functions declarations
double determinant(vector<vector<double>> matrix);
void worker(vector<vector<double>> submatrix, double& det);
double threaded_determinant(vector<vector<double>> matrix, int num_threads);

// Main program
int main() {
  vector<vector<double>> matrix = {{4, 3, 8, 2}, {4, 5, 6, 0}, {7, 8, 9, 8}, {4, 3, 8, 2}};
  int numberOfThreads = std::thread::hardware_concurrency();
  double det = threaded_determinant(matrix, matrix.size() % 2 == 0 ? numberOfThreads - 1 : numberOfThreads);
  cout << det << endl;
  return 0;
}

// ====== Functions implementations ======

// Function calculate determinant of matrix
double determinant(vector<vector<double>> matrix) {
  int size = matrix.size();

  if (size == 1) {
    return matrix[0][0];
  } else if (size == 2) {
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
  } else {
    double det = 0;
    for (int j = 0; j < size; j++) {
      vector<vector<double>> minor(size - 1, vector<double>(size - 1));
      for (int i = 1; i < size; i++) {
        for (int k = 0; k < size; k++) {
          if (k < j) {
            minor[i - 1][k] = matrix[i][k];
          } else if (k > j) {
            minor[i - 1][k - 1] = matrix[i][k];
          }
        }
      }

      double cofactor = (j % 2 == 0 ? 1 : -1) * determinant(minor);
      det += matrix[0][j] * cofactor;
    }

    return det;
  }
}

// Helper function used by single thread
void worker(vector<vector<double>> submatrix, double& det) {
  det = determinant(submatrix);
}

// Function calculate determinant of matrix by using threads
double threaded_determinant(vector<vector<double>> matrix, int num_threads) {
  int size = matrix.size();
  int step = size / num_threads;

  vector<thread> threads(num_threads);
  vector<double> detValues(num_threads, 0);

  for (int i = 0; i < num_threads; i++) {
    int start = i * step;
    int end = (i != num_threads-1 ? (i + 1) * step : size);
    vector<vector<double>> submatrix(end - start, vector<double>(size));
    for (int j = start; j < end; j++) {
      submatrix[j - start] = matrix[j];
    }

    threads[i] = thread(worker, submatrix, ref(detValues[i]));
  }

  for (auto& t : threads) {
    t.join();
  }

  double det = 0;
  for (auto val : detValues) {
    det += val;
  }

  return det;
}
