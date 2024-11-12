#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 100

// Circular buffer structure
typedef struct {
    unsigned int buffer[BUFFER_SIZE];
    int in;
    int out;
    int count;
    FILE *output_file;
    int cnt;
} CircularBuffer;

// Shared resources
CircularBuffer cb;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_produce = PTHREAD_COND_INITIALIZER;
pthread_cond_t can_consume = PTHREAD_COND_INITIALIZER;
int producer_done = 0;  // Flag to indicate producer has finished

// Initialize the circular buffer
void init_buffer(const char* output_filename) {
    cb.in = 0;
    cb.out = 0;
    cb.count = 0;
    cb.output_file = fopen(output_filename, "w");
    if (cb.output_file == NULL) {
        perror("Error opening output file");
        exit(1);
    }
}

// Write current buffer state to output file
void write_buffer_state() {
    fprintf(cb.output_file, "Buffer-State:[");
    if (cb.count > 0) {
        int index = cb.out;
        for (int i = 0; i < cb.count; i++) {
            fprintf(cb.output_file, "%u", cb.buffer[index]);
            if (i < cb.count - 1) fprintf(cb.output_file, ",");
            index =(index + 1) % BUFFER_SIZE;
        }
    }
    fprintf(cb.output_file, "]\n");
}

// Producer thread function
void* producer(void* arg) {
    FILE* input_file = fopen("input-part1.txt", "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(1);
    }

    unsigned int item;
    while (fscanf(input_file, "%u", &item) == 1) {
        if (item == 0) break;  // End of input

        pthread_mutex_lock(&mutex);
        while (cb.count == BUFFER_SIZE) pthread_cond_wait(&can_produce, &mutex);

        cb.buffer[cb.in] = item;
        cb.in = (cb.in + 1) % BUFFER_SIZE;
        cb.count++;
        pthread_cond_signal(&can_consume);
        pthread_mutex_unlock(&mutex);
    }

    fclose(input_file);

    // Signal that the producer is done
    pthread_mutex_lock(&mutex);
    producer_done = 1;
    pthread_cond_signal(&can_consume);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

// Consumer thread function
void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // Wait for items to consume or producer to finish
        while (cb.count == 0 && !producer_done) {
            pthread_cond_wait(&can_consume, &mutex);
        }

        if (cb.count == 0 && producer_done) {
            // Break the loop if buffer is empty and producer is done
            pthread_mutex_unlock(&mutex);
            break;
        }

        unsigned int item = cb.buffer[cb.out];
        cb.out = (cb.out + 1) % BUFFER_SIZE;
        cb.count--;

        fprintf(cb.output_file, "Consumed:[%u],", item);
        write_buffer_state();

        pthread_cond_signal(&can_produce);
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    // Initialize buffer and open output file
    init_buffer("output-part1.txt");

    // Create producer and consumer threads
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // Wait to finish
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // To Clean
    fclose(cb.output_file);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&can_produce);
    pthread_cond_destroy(&can_consume);

    return 0;
}
