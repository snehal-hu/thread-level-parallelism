#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define ARRAY_SIZE 100000000

double get_time()
{
    static LARGE_INTEGER frequency;
    static int initialized = 0;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        initialized = 1;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (double)counter.QuadPart / frequency.QuadPart;
}

int main()
{
    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));

    if (array == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    /*
     * Initialize the array.
     */
    for (long long i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = 1;
    }

    /*
     * Start timing only the computation.
     */
    double start = get_time();

    long long sum = 0;

    for (long long i = 0; i < ARRAY_SIZE; i++)
    {
        sum += array[i];
    }

    double end = get_time();

    printf("\n");
    printf("========================================\n");
    printf("       SEQUENTIAL EXECUTION\n");
    printf("========================================\n");
    printf("Array size        : %d\n", ARRAY_SIZE);
    printf("Number of threads : 1\n");
    printf("Sum               : %lld\n", sum);
    printf("Execution time    : %.6f seconds\n", end - start);
    printf("========================================\n");

    free(array);

    return 0;
}
