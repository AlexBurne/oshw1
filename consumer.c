// consumer.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define TABLE_SIZE 2
#define SHM_NAME "/shared_table"

// Same struct as producer
typedef struct {
    int table[TABLE_SIZE];
    int count;
    sem_t empty;
    sem_t full;
    sem_t mutex;
} shared_data;

int main() {
    // Access existing shared memory
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    shared_data *data = mmap(0, sizeof(shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (1) {
        sem_wait(&data->full);  // wait if no items
        sem_wait(&data->mutex); // enter critical section

        int item = data->table[--data->count];
        printf("Consumed item %d (count = %d)\n", item, data->count);

        sem_post(&data->mutex); // exit critical section
        sem_post(&data->empty); // signal space available

        sleep(2); 
    }

    return 0;
}
