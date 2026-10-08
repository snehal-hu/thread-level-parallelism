#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <windows.h>

#define NUM_TASKS 100000000
#define MAX_THREADS 12
#define CHUNK_SIZE 100000

long long *data;
long long next_index = 0;

pthread_mutex_t task_mutex;

typedef struct
{
    long long local_sum;
} ThreadData;

double current_time()
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);

    return (double)counter.QuadPart / (double)frequency.QuadPart;
}

void *worker(void *arg)
{
    ThreadData *thread_data = (ThreadData *)arg;

    long long local_sum = 0;

    while (1)
    {
        long long start;
        long long end;

        /* Get the next available chunk */
        pthread_mutex_lock(&task_mutex);

        if (next_index >= NUM_TASKS)
        {
            pthread_mutex_unlock(&task_mutex);
            break;
        }

        start = next_index;
        end = next_index + CHUNK_SIZE;

        if (end > NUM_TASKS)
        {
            end = NUM_TASKS;
        }

        next_index = end;

        pthread_mutex_unlock(&task_mutex);

        /* Process the assigned chunk */
        for (long long i = start; i < end; i++)
        {
            local_sum += data[i];
        }
    }

    thread_data->local_sum = local_sum;

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);

    if (num_threads < 1 || num_threads > MAX_THREADS)
    {
        printf("Number of threads must be between 1 and %d.\n",
               MAX_THREADS);
        return 1;
    }

    /* Allocate memory */
    data = (long long *)malloc(NUM_TASKS * sizeof(long long));

    if (data == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    /* Initialize array */
    for (long long i = 0; i < NUM_TASKS; i++)
    {
        data[i] = 1;
    }

    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];

    pthread_mutex_init(&task_mutex, NULL);

    next_index = 0;

    /* Start timing */
    double start_time = current_time();

    /* Create threads */
    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].local_sum = 0;

        if (pthread_create(&threads[i],
                           NULL,
                           worker,
                           &thread_data[i]) != 0)
        {
            printf("Failed to create thread %d.\n", i);

            free(data);
            pthread_mutex_destroy(&task_mutex);

            return 1;
        }
    }

    /* Wait for all threads */
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /* Calculate final sum */
    long long total_sum = 0;

    for (int i = 0; i < num_threads; i++)
    {
        total_sum += thread_data[i].local_sum;
    }

    /* Stop timing */
    double end_time = current_time();

    printf("\n===== Load Balanced Parallel Execution =====\n");
    printf("Array size        : %d\n", NUM_TASKS);
    printf("Number of threads : %d\n", num_threads);
    printf("Sum               : %lld\n", total_sum);
    printf("Execution time    : %.6f seconds\n",
           end_time - start_time);

    pthread_mutex_destroy(&task_mutex);
    free(data);

    return 0;
}
