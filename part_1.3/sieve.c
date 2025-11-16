#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 100
#define POISON -1
#define MAX_THREADS 10000

// track all filter threads
pthread_t thread_list[MAX_THREADS];
int thread_count = 0;
pthread_mutex_t thread_list_lock = PTHREAD_MUTEX_INITIALIZER;

void register_thread(pthread_t tid) {
    pthread_mutex_lock(&thread_list_lock);
    thread_list[thread_count++] = tid;
    pthread_mutex_unlock(&thread_list_lock);
}

// ---------------- queue ----------------
typedef struct {
    int buffer[BUFFER_SIZE];
    int head, tail, count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} BoundedQueue;

void init_queue(BoundedQueue* q) {
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

void push_queue(BoundedQueue* q, int value) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == BUFFER_SIZE)
        pthread_cond_wait(&q->not_full, &q->mutex);

    q->buffer[q->tail] = value;
    q->tail = (q->tail + 1) % BUFFER_SIZE;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

int pop_queue(BoundedQueue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0)
        pthread_cond_wait(&q->not_empty, &q->mutex);

    int v = q->buffer[q->head];
    q->head = (q->head + 1) % BUFFER_SIZE;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return v;
}

// ---------------- filter ----------------
typedef struct PrimeFilter {
    int prime_value;
    BoundedQueue* input_queue;
    BoundedQueue* next_queue;
    struct PrimeFilter* next_filter;
} PrimeFilter;

void* filter_worker(void* arg) {
    PrimeFilter* filter = (PrimeFilter*)arg;

    while (1) {
        int candidate = pop_queue(filter->input_queue);

        if (candidate == POISON) {
            if (filter->next_queue)
                push_queue(filter->next_queue, POISON);
            break;
        }

        if (filter->prime_value == 0) {
            filter->prime_value = candidate;
            printf("%d\n", filter->prime_value);
            fflush(stdout);
        } else {
            if (candidate % filter->prime_value != 0) {
                if (!filter->next_filter) {
                    filter->next_filter = malloc(sizeof(PrimeFilter));
                    filter->next_filter->prime_value = 0;
                    filter->next_filter->input_queue = malloc(sizeof(BoundedQueue));
                    init_queue(filter->next_filter->input_queue);
                    filter->next_filter->next_queue = NULL;
                    filter->next_filter->next_filter = NULL;

                    pthread_t tid;
                    pthread_create(&tid, NULL, filter_worker, filter->next_filter);
                    register_thread(tid);
                    filter->next_queue = filter->next_filter->input_queue;
                }
                push_queue(filter->next_queue, candidate);
            }
        }
    }

    return NULL;
}

// ---------------- generator ----------------
typedef struct { int limit; } GeneratorArgs;

void* generator_worker(void* arg) {
    int max_num = ((GeneratorArgs*)arg)->limit;

    PrimeFilter* first = malloc(sizeof(PrimeFilter));
    first->prime_value = 0;
    first->input_queue = malloc(sizeof(BoundedQueue));
    init_queue(first->input_queue);
    first->next_queue = NULL;
    first->next_filter = NULL;

    pthread_t tid;
    pthread_create(&tid, NULL, filter_worker, first);
    register_thread(tid);

    for (int i = 2; i <= max_num; i++)
        push_queue(first->input_queue, i);

    push_queue(first->input_queue, POISON);
    return NULL;
}

// ---------------- main ----------------
void run_experiments() {
    int limits[] = {10, 20, 50, 100, 200, 300, 500, 800, 1000, 2000, 3000, 5000};
    int count = sizeof(limits) / sizeof(limits[0]);

    //
    FILE *f = fopen("experiment_results.csv", "w");
    if (!f) {
        printf("ERROR: Cannot open CSV file.\n");
        return;
    }
    fprintf(f, "limit,time_seconds\n");
    fclose(f);

    for (int i = 0; i < count; i++) {
        int limit = limits[i];

        clock_t begin = clock();

        GeneratorArgs args;
        args.limit = limit;

        pthread_t gen_tid;
        pthread_create(&gen_tid, NULL, generator_worker, &args);
        pthread_join(gen_tid, NULL);

        double elapsed = (double)(clock() - begin) / CLOCKS_PER_SEC;

        FILE *f2 = fopen("experiment_results.csv", "a");
        if (!f2) {
            printf("ERROR\n");
            return;
        }

        fprintf(f2, "%d,%.8f\n", limit, elapsed);
        fclose(f2);
    }

    printf("All experiment results saved \n");
}


// ---------------- main ----------------
int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s 3 <limit>\n", argv[0]);
        return 1;
    }

    int limit = atoi(argv[2]);

    pthread_t gen_tid;
    GeneratorArgs args = {limit};

    pthread_create(&gen_tid, NULL, generator_worker, &args);
    pthread_join(gen_tid, NULL);

    // wait for all filter threads
    for (int i = 0; i < thread_count; i++)
        pthread_join(thread_list[i], NULL);

    return 0;
}

// int main() {
//    run_experiments();
//    return 0;
//}
