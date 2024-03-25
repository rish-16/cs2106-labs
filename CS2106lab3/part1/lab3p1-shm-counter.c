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
    sem_t *semaphore;
    int *arr;

    shmid = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
    arr = (int *) shmat(shmid, NULL, 0);
    *arr[0] = *semaphore;
    sem_init(semaphore, 1, 0);

    if (shmid == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    } else {
        printf("Shared Memory Id = %d\n", shmid);
    }

    // Attach the shared memory segment
    // if (*semaphore == (int *)-1) {
    //     printf("Cannot attach shared memory!\n");
    //     exit(1);
    // }

    *arr = 0; // counter

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

        printf("Child %d starts\n", i + 1);
        
        // Simulate some work
        for (int j = 0; j < 5; j++) {
            sem_wait(semaphore);
            *arr = *arr + 1; // update counter
            printf("Child %d increment counter %d\n", i + 1, *arr);
            sem_post(semaphore);
            fflush(stdout);
            usleep(250000);
        }

        printf("Child %d finishes with counter %d\n", i + 1, *arr);

        exit(EXIT_SUCCESS);
    }

    // Parent process
    for (int i = 0; i < NUM_CHILDREN; i++) {
        wait(NULL);
    }

    // Print the final value of thecounter
    printf("Final counter value: %d\n", *arr);

    // Detach the shared memory segment
    if (shmdt(arr) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }

    // Destroy semaphore
    sem_destroy(semaphore);
    free(semaphore);

    return 0;
}
