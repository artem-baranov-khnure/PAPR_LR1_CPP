#include <windows.h>
#include "TasksPartTwo.h"

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    const int repeats = 1;

    runTask7({ 512, 1024, 2048 }, repeats);

    runTask9(1024, repeats);

    return 0;
}