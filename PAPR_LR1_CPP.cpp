#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cstdlib>
#include <windows.h>
#include <omp.h>
#include <chrono>
#include <intrin.h>
#include <ctime>

using namespace std;
using namespace std::chrono;





// Task 2 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// При виконанні використалися "Пз-1.pdf", ст. 19-22

// time()
double GetAccuracyTime() {
    // https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/reference/time-time32-time64?view=msvc-170
    time_t start = time(NULL);
    time_t finish;

    do {
        finish = time(NULL);
    } while (finish - start == 0);

    return (double)(finish - start);
}

// clock()
double GetAccuracyClock() {
    // https://en.cppreference.com/cpp/chrono/c/clock
    clock_t start = clock();
    clock_t finish;

    do {
        finish = clock();
    } while (finish - start == 0);

    return (double)(finish - start) / CLOCKS_PER_SEC;
}

// FILETIME to 64-bit integer
// https://stackoverflow.com/questions/1566645/filetime-to-int64
ULONGLONG FileTimeToUInt64(const FILETIME& file_time) {
    ULARGE_INTEGER ul_integer_time;

    ul_integer_time.LowPart = file_time.dwLowDateTime;
    ul_integer_time.HighPart = file_time.dwHighDateTime;

    return ul_integer_time.QuadPart;
}

// GetSystemTimeAsFileTime
// https://stackoverflow.com/questions/32470442/c-beginner-how-to-use-getsystemtimeasfiletime
double GetAccuracySystemTime() {
    FILETIME start, finish;
    GetSystemTimeAsFileTime(&start);

    do {
        GetSystemTimeAsFileTime(&finish);
    } while (FileTimeToUInt64(finish) - FileTimeToUInt64(start) == 0);

    return (double)(FileTimeToUInt64(finish) - FileTimeToUInt64(start)) / 1e7;
}

// GetSystemTimePreciseAsFileTime
double GetAccuracyPreciseSystemTime() {
    FILETIME start, finish;
    GetSystemTimePreciseAsFileTime(&start);

    do {
        GetSystemTimePreciseAsFileTime(&finish);
    } while (FileTimeToUInt64(finish) - FileTimeToUInt64(start) == 0);

    return (double)(FileTimeToUInt64(finish) - FileTimeToUInt64(start)) / 1e7;
}

// GetTickCount
// https://learn.microsoft.com/ru-ru/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount
double GetAccuracyTickCount() {
    DWORD start = GetTickCount();
    DWORD finish;

    do {
        finish = GetTickCount();
    } while (finish - start == 0);

    return (finish - start) / 1000.0;
}

// __rdtsc
unsigned __int64 GetAccuracyRdtsc() {
    // https://learn.microsoft.com/ru-ru/cpp/intrinsics/rdtsc?view=msvc-170
    unsigned __int64 start = __rdtsc();
    unsigned __int64 finish;

    do {
        finish = __rdtsc();
    } while (finish - start == 0);

    return finish - start;
}


// Rdtsc (asm) ??????????????????????????????????????????????????????????????????????
// тут на ассемблере писать надо! это точно нам нужно? надо спросить


// QueryPerformanceCounter
double GetAccuracyQPC() {
    LARGE_INTEGER start, finish, freq;
    // https://learn.microsoft.com/ru-ru/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
    QueryPerformanceFrequency(&freq);
    // https://learn.microsoft.com/ru-ru/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter
    QueryPerformanceCounter(&start);

    do {
        QueryPerformanceCounter(&finish);
    } while (finish.QuadPart - start.QuadPart == 0);

    return (double)(finish.QuadPart - start.QuadPart) / freq.QuadPart;
}

// std::chrono::high_resolution_clock
double GetAccuracyChrono() {
    // https://learn.microsoft.com/ru-ru/cpp/standard-library/high-resolution-clock-struct?view=msvc-170
    high_resolution_clock::time_point start, finish;
    start = high_resolution_clock::now();

    do {
        finish = high_resolution_clock::now();
    } while ((finish - start).count() == 0);

    return duration_cast<duration<double>>(finish - start).count();
}

// omp_get_wtime
double GetAccuracyOMP() {
    // https://www.openmp.org/spec-html/5.0/openmpsu160.html
    double start = omp_get_wtime();
    double finish;

    do {
        finish = omp_get_wtime();
    } while (finish - start == 0.0);

    return finish - start;
}


// При виконанні використалися "Пз-1.pdf", ст. 19-20
template <class Function>
double GetMinAccuracy(Function&& measure, int repeat = 10) {
    double min_val = measure();
    for (int i = 1; i < repeat; i++) {
        double val = measure();
        if (val < min_val) {
            min_val = val;
        }
    }
    return min_val;
}


template <class Function>
unsigned __int64 GetMinAccuracyInt(Function&& measure, int repeat = 10) {
    unsigned __int64 min_val = measure();
    for (int i = 1; i < repeat; i++) {
        unsigned __int64 val = measure();
        if (val < min_val) {
            min_val = val;
        }
    }
    return min_val;
}



// Task 3 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void ArrayGenerator(int* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        arr[i] = rand() % 100;
    }
}

void AddValueToArray(volatile int* arr, size_t size, int value) {
    for (size_t i = 0; i < size; i++) {
        arr[i] += value;
    }
}

template <class Function>
double time_call_omp(Function&& f) {
    double begin = omp_get_wtime();
    f();
    return omp_get_wtime() - begin;
}



// Task 1 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void PrintAsSystemTime(int32_t unix_time) {

    // magic number taken from:
    // https://learn.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time
    long long time_value = (long long)unix_time * 10000000LL + 116444736000000000LL;

    ULARGE_INTEGER ul_integer_time;
    ul_integer_time.QuadPart = time_value;


    FILETIME file_time;
    // https://learn.microsoft.com/en-us/windows/win32/api/minwinbase/ns-minwinbase-filetime
    file_time.dwLowDateTime = ul_integer_time.LowPart;
    file_time.dwHighDateTime = ul_integer_time.HighPart;


    SYSTEMTIME system_time;
    // https://learn.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-filetimetosystemtime
    FileTimeToSystemTime(&file_time, &system_time);


    printf("%02d.%02d.%d  %02d:%02d:%02d\n", system_time.wDay, system_time.wMonth,
        system_time.wYear, system_time.wHour, system_time.wMinute, system_time.wSecond);
}




int main()
{
    // https://www.w3schools.com/c/ref_stdlib_srand.php
    srand((unsigned int)time(NULL));
    // https://learn.microsoft.com/ru-ru/cpp/standard-library/using-insertion-operators-and-controlling-format?view=msvc-170
    // https://en.cppreference.com/cpp/io/manip/setprecision
    cout << fixed << setprecision(9);


    cout << "Task 1 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

    int32_t max_time = 0x7FFFFFFF;
    int32_t overflow_time = max_time + 1;

    time_t start_datetime = 0;
    time_t final_datetime = max_time;

    // https://cplusplus.com/reference/ctime/tm/
    tm timestamp_s;
    tm timestamp_f;

    // https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/reference/gmtime-s-gmtime32-s-gmtime64-s?view=msvc-170
    gmtime_s(&timestamp_s, &start_datetime);
    gmtime_s(&timestamp_f, &final_datetime);

    char buffer[80];
    // https://learn.microsoft.com/ru-ru/cpp/c-runtime-library/reference/strftime-wcsftime-strftime-l-wcsftime-l?view=msvc-170
    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", &timestamp_s);
    cout << "Start date: " << buffer << endl;

    strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", &timestamp_f);
    cout << "Final date: " << buffer << endl;


    cout << "\nFinal date (SystemTime)   : ";
    PrintAsSystemTime(max_time);

    cout << "Overflow date (SystemTime): ";
    PrintAsSystemTime(overflow_time);



    cout << "\n\n\n" << endl;
    cout << "Task 2 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

    cout << "time                   : " << GetMinAccuracy(GetAccuracyTime) << " s\n";
    cout << "clock                  : " << GetMinAccuracy(GetAccuracyClock) << " s\n";
    cout << "SystemTimeAsFileTime   : " << GetMinAccuracy(GetAccuracySystemTime) << " s\n";
    cout << "PreciseSystemTime      : " << GetMinAccuracy(GetAccuracyPreciseSystemTime) << " s\n";
    cout << "GetTickCount           : " << GetMinAccuracy(GetAccuracyTickCount) << " s\n";
    cout << "__rdtsc                : " << GetMinAccuracyInt(GetAccuracyRdtsc) << " cycles\n";
    cout << "QueryPerformanceCounter: " << GetMinAccuracy(GetAccuracyQPC) << " s\n";
    cout << "std::chrono            : " << GetMinAccuracy(GetAccuracyChrono) << " s\n";
    cout << "omp_get_wtime          : " << GetMinAccuracy(GetAccuracyOMP) << " s\n";



    cout << "\n\n\n" << endl;
    cout << "Task 3 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;

    const size_t arr_size = 1000;
    int* arr = new int[arr_size];
    ArrayGenerator(arr, arr_size);


    unsigned __int64 min_rdtsc_cycles = _UI64_MAX;
    double min_qpc_sec = 1.0e9;

    LARGE_INTEGER frequency, start_q, end_q;
    // https://learn.microsoft.com/ru-ru/windows/win32/api/profileapi/nf-profileapi-queryperformancefrequency
    QueryPerformanceFrequency(&frequency);

    for (int i = 0; i < 10; i++) {
        // QueryPerformanceCounter
        QueryPerformanceCounter(&start_q);
        AddValueToArray((volatile int*)arr, arr_size, 5);
        QueryPerformanceCounter(&end_q);

        double seconds = (double)(end_q.QuadPart - start_q.QuadPart) / frequency.QuadPart;
        if (seconds < min_qpc_sec) {
            min_qpc_sec = seconds;
        }

        // __rdtsc
        unsigned __int64 startR = __rdtsc();
        AddValueToArray((volatile int*)arr, arr_size, 5);
        unsigned __int64 finishR = __rdtsc();

        unsigned __int64 cycles = finishR - startR;
        if (cycles < min_rdtsc_cycles) {
            min_rdtsc_cycles = cycles;
        }
    }

    cout << "__rdtsc                : " << min_rdtsc_cycles << " cycles" << endl;
    cout << "QueryPerformanceCounter: " << min_qpc_sec * 1e6 << " microseconds" << endl;

    delete[] arr;


    cout << endl;


    cout << "\n\n\n" << endl;
    cout << "Task 4 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << endl;


    const size_t arr_sizes[3] = { 100000, 200000, 300000 };
    double absolute_time[3];
    long long relative_count[3];

    for (int i = 0; i < 3; i++) {
        int* arr = new int[arr_sizes[i]];
        ArrayGenerator(arr, arr_sizes[i]);


        const int repeat_count = 50;
        double min_work_time = 1.0e9;
        for (int r = 0; r < 10; r++) {

            double t = time_call_omp([&] {
                for (int k = 0; k < repeat_count; k++) {
                    AddValueToArray((volatile int*)arr, arr_sizes[i], 2);
                }
                });

            double single_call_time = t / repeat_count;

            if (single_call_time < min_work_time) {
                min_work_time = single_call_time;
            }
        }
        absolute_time[i] = min_work_time;


        long long count = 0;
        DWORD start_tick = GetTickCount();
        while (GetTickCount() - start_tick < 2000) {
            AddValueToArray((volatile int*)arr, arr_sizes[i], 2);
            count++;
        }

        relative_count[i] = count;

        cout << "Current array size = " << arr_sizes[i]
            << "   absolute time = " << absolute_time[i] << " s"
            << "   relative count = " << count << endl;

        delete[] arr;
    }

    // https://en.cppreference.com/cpp/io/manip/setprecision
    cout << setprecision(4);
    cout << "\nRatios, absolute measurement: " << endl;
    cout << "T(200000)/T(100000) = " << absolute_time[1] / absolute_time[0] << endl;
    cout << "T(300000)/T(100000) = " << absolute_time[2] / absolute_time[0] << endl;



    cout << "\nRatios, relative measurement: " << endl;
    cout << "T(200000)/T(100000) = " << (double)relative_count[0] / relative_count[1] << endl;
    cout << "T(300000)/T(100000) = " << (double)relative_count[0] / relative_count[2] << endl;

    cout << "\n\n\n" << endl;


    system("pause");
    return 0;
}