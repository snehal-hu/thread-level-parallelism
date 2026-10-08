#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>

#define ARRAY_SIZE 100000000
#define MAX_THREADS 12

int *array;

typedef struct
{
    int thread_id;
    long long start;
    long long end;
    long long partial_sum;
} ThreadData;

double get_time()
{
    static LARGE_INTEGER frequency;
    static int initialized = 0;

    LARGE_INTEGER counter;

    if (!initialized)
    {
        QueryPerformanceFrequency(&frequency);
        initialized = 1;
    }

    QueryPerformanceCounter(&counter);

    return (double)counter.QuadPart /
           (double)frequency.QuadPart;
}

void *worker(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    long long sum = 0;

    for (long long i = data->start; i < data->end; i++)
    {
        sum += array[i];
    }

    data->partial_sum = sum;

    return NULL;
}

int main(int argc, char *argv[])
{
    int num_threads = 4;

    if (argc > 1)
    {
        num_threads = atoi(argv[1]);
    }

    if (num_threads < 1 || num_threads > MAX_THREADS)
    {
        printf("Number of threads must be between 1 and %d.\n",
               MAX_THREADS);
        return 1;
    }

    array = malloc((size_t)ARRAY_SIZE * sizeof(int));

    if (array == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    for (long long i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = 1;
    }

    pthread_t threads[MAX_THREADS];
    ThreadData data[MAX_THREADS];

    long long chunk = ARRAY_SIZE / num_threads;

    double start_time = get_time();

    for (int i = 0; i < num_threads; i++)
    {
        data[i].thread_id = i;
        data[i].start = i * chunk;

        if (i == num_threads - 1)
        {
            data[i].end = ARRAY_SIZE;
        }
        else
        {
            data[i].end = (i + 1) * chunk;
        }

        data[i].partial_sum = 0;

        if (pthread_create(&threads[i], NULL, worker, &data[i]) != 0)
        {
            printf("Failed to create thread %d.\n", i);
            free(array);
            return 1;
        }
    }

    long long total_sum = 0;

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
        total_sum += data[i].partial_sum;
    }

    double end_time = get_time();

    printf("\n========================================\n");
    printf("       PARALLEL EXECUTION\n");
    printf("========================================\n");
    printf("Array size        : %d\n", ARRAY_SIZE);
    printf("Number of threads : %d\n", num_threads);
    printf("Sum               : %lld\n", total_sum);
    printf("Execution time    : %.6f seconds\n",
           end_time - start_time);
    printf("========================================\n");

    free(array);

    return 0;
}
