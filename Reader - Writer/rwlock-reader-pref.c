#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

#define OUTPUT_FILE "output-reader-pref.txt"
#define SHARED_FILE "shared-file.txt"

sem_t resource_access; 
sem_t read_count_access;
int read_count = 0; 

void *reader(void *arg) {
    // Entering critical section to update reader count
    sem_wait(&read_count_access);
    read_count++;
    
    if (read_count == 1) sem_wait(&resource_access); // First reader locks the resource
    
    sem_post(&read_count_access); 

    FILE *output_fp = fopen(OUTPUT_FILE, "a");
    if (output_fp != NULL) {
        fprintf(output_fp, "Reading,Number-of-readers-present:[%d]\n", read_count);
        fclose(output_fp);
    }

    // Reading section
    FILE *shared_fp = fopen(SHARED_FILE, "r");
    if (shared_fp != NULL) fclose(shared_fp);
    

    // Exiting critical section to update reader count
    sem_wait(&read_count_access);
    read_count--;
    if (read_count == 0) sem_post(&resource_access); 
    
    sem_post(&read_count_access);

    return NULL;
}



void *writer(void *arg) {
    // Wait until no readers or writers are accessing the resource
    sem_wait(&resource_access);

    // Writing section
    FILE *output_fp = fopen(OUTPUT_FILE, "a");
    if (output_fp != NULL) {
        fprintf(output_fp, "Writing,Number-of-readers-present:[%d]\n", read_count);
        fclose(output_fp);
    }
    FILE *shared_fp = fopen(SHARED_FILE, "a");
    if (shared_fp != NULL) {
        fprintf(shared_fp, "Hello world!\n");
        fclose(shared_fp);
    }

    // Release the lock
    sem_post(&resource_access);

    return NULL;
}

int main(int argc, char **argv) {

    //Do not change the code below to spawn threads
    if(argc != 3) return 1;
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    pthread_t readers[n], writers[m];

    // Initialize semaphores
    sem_init(&resource_access, 0, 1);
    sem_init(&read_count_access, 0, 1);

    // Create reader and writer threads
    for (int i = 0; i < n; i++) pthread_create(&readers[i], NULL, reader, NULL);
    for (int i = 0; i < m; i++) pthread_create(&writers[i], NULL, writer, NULL);

    // Wait for all threads to complete
    for (int i = 0; i < n; i++) pthread_join(readers[i], NULL);
    for (int i = 0; i < m; i++) pthread_join(writers[i], NULL);

    // Destroy semaphores
    sem_destroy(&resource_access);
    sem_destroy(&read_count_access);

    return 0;
}
