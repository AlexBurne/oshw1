// producer.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define TABLE_SIZE 2
#define SHM_NAME "/shared_table"

// Shared memory structure
typedef struct {
    int table[TABLE_SIZE];
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
} shared_data;

int main() {
    // Create shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_data));
    shared_data *data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // Initialize shared data and semaphores
    data->count = 0;
    sem_init(&data->empty, 1, TABLE_SIZE); // space for 2 items
    sem_init(&data->full, 1, 0);           // no items at start
    sem_init(&data->mutex, 1, 1);          // mutual exclusion

    int item = 1;
    while (1) {
        sem_wait(&data->empty); // wait if table full
        sem_wait(&data->mutex); // enter critical section

        data->table[data->count++] = item;
        printf("Produced item %d (count = %d)\n", item, data->count);

        sem_post(&data->mutex); // exit critical section
        sem_post(&data->full);  // signal item available

        item++;
        sleep(1); 
    }

    return 0;
}
