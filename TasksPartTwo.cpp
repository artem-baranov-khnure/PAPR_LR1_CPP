#include "TasksPartTwo.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <random>
#include <cstdint>
#include <limits>
#include <functional>

using namespace std;

namespace Part2
{
    template<typename T>
    T** allocateArray(size_t n)
    {
        T** arr = new T * [n];
        for (size_t i = 0; i < n; ++i) {
            arr[i] = new T[n];
        }
        return arr;
    }

    template<typename T>
    void freeArray(T** arr, size_t n)
    {
        for (size_t i = 0; i < n; ++i) {
            delete[] arr[i];
        }
        delete[] arr;
    }

    template<typename T>
    T** multiplyArrays(T** A, T** B, size_t n)
    {
        T** result = allocateArray<T>(n);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                T sum = 0;
                for (size_t k = 0; k < n; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    template<typename T>
    T** generateRandomArray(size_t n, unsigned seed)
    {
        T** values = allocateArray<T>(n);
        mt19937 randomEngine(seed);
        uniform_real_distribution<double> distribution(0.0, 9.0);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                values[i][j] = static_cast<T>(distribution(randomEngine));
            }
        }
        return values;
    }

    template<typename T>
    class Matrix
    {
    public:
        Matrix(size_t n)
        {
            matrixSize = n;
            matrixArray = new T[n * n];
        }

        ~Matrix()
        {
            delete[] matrixArray;
        }

        T& at(size_t row, size_t col)
        {
            return matrixArray[row * matrixSize + col];
        }

        const T& at(size_t row, size_t col) const
        {
            return matrixArray[row * matrixSize + col];
        }

        void multiply(const Matrix<T>& other, Matrix<T>& result) const
        {
            for (size_t i = 0; i < matrixSize; ++i) {
                for (size_t j = 0; j < matrixSize; ++j) {
                    T sum = 0;
                    for (size_t k = 0; k < matrixSize; ++k) {
                        sum += at(i, k) * other.at(k, j);
                    }
                    result.at(i, j) = sum;
                }
            }
        }

    private:
        size_t matrixSize;
        T* matrixArray;
    };

    template<typename T>
    void fillRandomMatrix(Matrix<T>& matrix, size_t n, unsigned seed)
    {
        mt19937 randomEngine(seed);
        uniform_real_distribution<double> distribution(0.0, 9.0);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                matrix.at(i, j) = static_cast<T>(distribution(randomEngine));
            }
        }
    }

    static double measureMinTime(const function<void()>& codeToMeasure, int repeats)
    {
        double minTime = (numeric_limits<double>::max)();

        for (int i = 0; i < repeats; ++i) {
            auto start = chrono::high_resolution_clock::now();
            codeToMeasure();
            auto end = chrono::high_resolution_clock::now();

            double elapsedSeconds = chrono::duration<double>(end - start).count();
            if (elapsedSeconds < minTime) {
                minTime = elapsedSeconds;
            }
        }
        return minTime;
    }

    struct ComparisonResult {
        double timeWithoutObjects;
        double timeWithObjects;
    };

    template<typename T>
    ComparisonResult compareArrayVsObject(size_t n, int repeats)
    {
        ComparisonResult result;

        T** A = generateRandomArray<T>(n, 1);
        T** B = generateRandomArray<T>(n, 2);

        result.timeWithoutObjects = measureMinTime([&]() {
            T** product = multiplyArrays(A, B, n);
            freeArray(product, n);
            }, repeats);

        freeArray(A, n);
        freeArray(B, n);

        Matrix<T> matrixA(n);
        fillRandomMatrix(matrixA, n, 1);
        Matrix<T> matrixB(n);
        fillRandomMatrix(matrixB, n, 2);

        result.timeWithObjects = measureMinTime([&]() {
            Matrix<T> product(n);
            matrixA.multiply(matrixB, product);
            }, repeats);

        return result;
    }

    static const int COL_SIZE = 10;
    static const int COL_TIME = 20;
    static const int COL_TYPE = 12;

    static void printSeparator(int totalWidth)
    {
        cout << string(totalWidth, '-') << "\n";
    }

    static void printTask7(const vector<size_t>& sizes, int repeats)
    {
        cout << "\n--- Завдання 7: множення матриць (double) ---\n";

        printSeparator(COL_SIZE + COL_TIME * 2);
        cout << left << setw(COL_SIZE) << "Розмір"
            << right << setw(COL_TIME) << "без об'єктів (с)"
            << right << setw(COL_TIME) << "з об'єктами (с)" << "\n";
        printSeparator(COL_SIZE + COL_TIME * 2);

        double previousTimeRaw = 0.0;
        double previousTimeObj = 0.0;
        size_t previousN = 0;

        cout << fixed << setprecision(6);

        for (size_t n : sizes) {
            ComparisonResult r = compareArrayVsObject<double>(n, repeats);

            cout << left << setw(COL_SIZE) << n
                << right << setw(COL_TIME) << r.timeWithoutObjects
                << right << setw(COL_TIME) << r.timeWithObjects << "\n";

            if (previousN != 0) {
                double sizeRatio = static_cast<double>(n) / previousN;
                double theoreticalRatio = sizeRatio * sizeRatio * sizeRatio;

                cout << setprecision(3)
                    << "--- T(" << n << ") / T(" << previousN << "): "
                    << "без об'єктів = " << (r.timeWithoutObjects / previousTimeRaw)
                    << ", з об'єктами = " << (r.timeWithObjects / previousTimeObj)
                    << ", теоретично n^3 дає = " << theoreticalRatio
                    << "\n";
                cout << setprecision(6);
            }

            previousTimeRaw = r.timeWithoutObjects;
            previousTimeObj = r.timeWithObjects;
            previousN = n;
        }
        printSeparator(COL_SIZE + COL_TIME * 2);
    }

    template<typename T>
    void printTypeTestResult(const char* typeName, size_t n, int repeats)
    {
        ComparisonResult r = compareArrayVsObject<T>(n, repeats);
        cout << fixed << setprecision(6)
            << left << setw(COL_TYPE) << typeName
            << right << setw(COL_TIME) << r.timeWithoutObjects
            << right << setw(COL_TIME) << r.timeWithObjects << "\n";
    }

    static void printTask9(size_t n, int repeats)
    {
        cout << "\n--- Завдання 9: вплив типу даних, n = " << n << " ---\n";

        printSeparator(COL_TYPE + COL_TIME * 2);
        cout << left << setw(COL_TYPE) << "Тип"
            << right << setw(COL_TIME) << "без об'єктів (с)"
            << right << setw(COL_TIME) << "з об'єктами (с)" << "\n";
        printSeparator(COL_TYPE + COL_TIME * 2);

        printTypeTestResult<int8_t>("int8_t", n, repeats);
        printTypeTestResult<int16_t>("int16_t", n, repeats);
        printTypeTestResult<int32_t>("int32_t", n, repeats);
        printTypeTestResult<int64_t>("int64_t", n, repeats);
        printTypeTestResult<float>("float", n, repeats);
        printTypeTestResult<double>("double", n, repeats);

        printSeparator(COL_TYPE + COL_TIME * 2);
    }

}

void runTask7(const vector<size_t>& sizes, int repeats)
{
    Part2::printTask7(sizes, repeats);
}

void runTask9(size_t n, int repeats)
{
    Part2::printTask9(n, repeats);
}