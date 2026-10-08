#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>

#define ARRAY_SIZE 100000000
#define MAX_THREADS 12

int *array;

/* Shared variable */
long long shared_sum = 0;

/* Mutex for protecting shared_sum */
pthread_mutex_t sum_mutex;

typedef struct
{
    int thread_id;
    long long start;
    long long end;
} ThreadData;

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

void *calculate_sum(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    long long local_sum = 0;

    /*
     * Each thread calculates its portion
     * independently.
     */
    for (long long i = data->start; i < data->end; i++)
    {
        local_sum += array[i];
    }

    /*
     * Critical section.
     * Only one thread can update shared_sum
     * at a time.
     */
    pthread_mutex_lock(&sum_mutex);

    shared_sum += local_sum;

    pthread_mutex_unlock(&sum_mutex);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        printf("Example: %s 4\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads < 1 || num_threads > MAX_THREADS)
    {
        printf("Number of threads must be between 1 and %d.\n",
               MAX_THREADS);
        return 1;
    }

    array = (int *)malloc(ARRAY_SIZE * sizeof(int));

    if (array == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    /*
     * Initialize array.
     */
    for (long long i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = 1;
    }

    /*
     * Initialize mutex.
     */
    if (pthread_mutex_init(&sum_mutex, NULL) != 0)
    {
        printf("Mutex initialization failed.\n");
        free(array);
        return 1;
    }

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    long long chunk_size = ARRAY_SIZE / num_threads;

    double start_time = get_time();

    /*
     * Create threads.
     */
    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].thread_id = i;
        thread_data[i].start = i * chunk_size;

        if (i == num_threads - 1)
        {
            thread_data[i].end = ARRAY_SIZE;
        }
        else
        {
            thread_data[i].end = (i + 1) * chunk_size;
        }

        int result = pthread_create(
            &threads[i],
            NULL,
            calculate_sum,
            &thread_data[i]
        );

        if (result != 0)
        {
            printf("Error creating thread %d.\n", i);

            pthread_mutex_destroy(&sum_mutex);
            free(array);

            return 1;
        }
    }

    /*
     * Wait for all threads.
     */
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    double end_time = get_time();

    printf("\n");
    printf("========================================\n");
    printf("      PARALLEL + MUTEX EXECUTION\n");
    printf("========================================\n");
    printf("Array size        : %d\n", ARRAY_SIZE);
    printf("Number of threads : %d\n", num_threads);
    printf("Sum               : %lld\n", shared_sum);
    printf("Execution time    : %.6f seconds\n",
           end_time - start_time);
    printf("Synchronization   : pthread_mutex\n");
    printf("========================================\n");

    pthread_mutex_destroy(&sum_mutex);

    free(array);

    return 0;
}
