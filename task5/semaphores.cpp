#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFER_SIZE 100
#define SHARED_MEMORY_SIZE sizeof(int) * BUFFER_SIZE

int *buffer;

sem_t *empty_semaphore;
sem_t *full_semaphore;
sem_t *mutex_semaphore;

void *producer(void *arg) {
    for (int i = 0; i < 100; i++) {
        sem_wait(empty_semaphore); 
        sem_wait(mutex_semaphore); 
        
        buffer[i % BUFFER_SIZE] = i;
        std::cout << "Producer added " << i << " in shared_memory\n";
        
        sem_post(mutex_semaphore); 
        sem_post(full_semaphore);  
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    for (int i = 0; i < 100; i++) {
        sem_wait(full_semaphore);  
        sem_wait(mutex_semaphore); 
        
        int item = buffer[i % BUFFER_SIZE];
        std::cout << "Consumer retrieved " << item << " from buffer\n";
        
        sem_post(mutex_semaphore); 
        sem_post(empty_semaphore); 
    }
    pthread_exit(NULL);
}

int main() {
    int shared_memory_id = shmget(IPC_PRIVATE, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shared_memory_id < 0) {
        std::cerr << "Error creating shared memory" << std:: endl ;
        return 1;
    }
    
    buffer = (int *) shmat(shared_memory_id, NULL, 0);
    
    empty_semaphore = sem_open("/empty_semaphore", O_CREAT, 0644, BUFFER_SIZE);
    full_semaphore = sem_open("/full_semaphore", O_CREAT, 0644, 0);
    mutex_semaphore = sem_open("/mutex_semaphore", O_CREAT, 0644, 1);
    
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    shmdt(buffer);
    shmctl(shared_memory_id, IPC_RMID, NULL);
    sem_unlink("/empty_semaphore");
    sem_unlink("/full_semaphore");
    sem_unlink("/mutex_semaphore");
    
    return 0;
}
