#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define NUM_CHILDREN 5

int main() {

    int shmid, *shm;
    void *shared_memory; // Pointer to shared memory
    int *counter;
    sem_t *semaphores;

    int size = sizeof(int) + NUM_CHILDREN*sizeof(sem_t);
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    
    if (shmid == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    } else {
        printf("Shared Memory Id = %d\n", shmid);
    }

    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    counter = (int *) shared_memory;
    semaphores = (sem_t *) (shared_memory + sizeof(int));

    *counter = 0;
    for (int k = 0; k < NUM_CHILDREN; k++) {
        sem_init(&semaphores[k], 1, 0); // Initialize semaphore with value 1
    }

    // int counter = 0, i;
    int i;
    pid_t pid;

    for (i = 0; i < NUM_CHILDREN; i++) {
        pid = fork();
        if (pid == 0) break;
    }

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process

        sem_wait(&semaphores[i]);

        printf("Child %d starts\n", i + 1);

        // Simulate some work
        for (int j = 0; j < 5; j++) {
            *counter = *counter + 1; // update counter
            printf("Child %d increment counter %d\n", i + 1, *counter);
            fflush(stdout);
            usleep(250000);
        }

        printf("Child %d finishes with counter %d\n", i + 1, *counter);

        sem_destroy(&semaphores[i]);
        sem_post(&semaphores[i+1]);
        shmdt((char *) counter);
        exit(EXIT_SUCCESS);
    }

    // Parent process
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Print the final value of thecounter
    printf("Final counter value: %d\n", *counter);

    // Detach the shared memory segment
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(1);
    }

    // for (int k = 0; k < NUM_CHILDREN; k++) {
    //     if (shmdt(&semaphores[k]) == -1) {
    //         perror("shmdt");
    //         exit(1);
    //     }
    //     sem_destroy(&semaphores[k]);
    // }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }

    return 0;
}
