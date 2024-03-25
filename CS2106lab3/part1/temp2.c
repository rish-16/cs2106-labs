#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SHARED_MEMORY_KEY 12345 // Change this to any unique key value
#define NUM_CHILDREN 5
#define NUM_INCREMENTS 10

typedef struct
{
    int counter;
    sem_t mutex;
    sem_t turnstile;
} SharedData;

void child_process(SharedData *shared_memory, int id)
{
    for (int i = 0; i < NUM_INCREMENTS; i++)
    {
        sem_wait(&(shared_memory->turnstile)); // Wait for turnstile
        sem_post(&(shared_memory->turnstile)); // Pass turnstile to next process

        sem_wait(&(shared_memory->mutex)); // Wait for access to the counter
        shared_memory->counter++;
        printf("Child %d incremented counter to: %d\n", id, shared_memory->counter);
        sem_post(&(shared_memory->mutex)); // Release access to the counter
    }
}

int main()
{
    int shmid;
    SharedData *shared_memory;

    // Create shared memory
    shmid = shmget(SHARED_MEMORY_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach shared memory
    shared_memory = (SharedData *)shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1)
    {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize the counter
    shared_memory->counter = 0;

    // Initialize mutex semaphore
    if (sem_init(&(shared_memory->mutex), 1, 1) == -1)
    {
        perror("sem_init(mutex)");
        exit(EXIT_FAILURE);
    }

    // Initialize turnstile semaphore
    if (sem_init(&(shared_memory->turnstile), 1, 0) == -1)
    {
        perror("sem_init(turnstile)");
        exit(EXIT_FAILURE);
    }

    // Create child processes
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            child_process(shared_memory, i);
            exit(0);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < NUM_CHILDREN; i++)
    {
        wait(NULL);
    }

    // Detach shared memory
    if (shmdt(shared_memory) == -1)
    {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Remove shared memory
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    // Destroy mutex semaphore
    if (sem_destroy(&(shared_memory->mutex)) == -1)
    {
        perror("sem_destroy(mutex)");
        exit(EXIT_FAILURE);
    }

    // Destroy turnstile semaphore
    if (sem_destroy(&(shared_memory->turnstile)) == -1)
    {
        perror("sem_destroy(turnstile)");
        exit(EXIT_FAILURE);
    }

    return 0;
}
