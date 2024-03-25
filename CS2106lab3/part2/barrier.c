#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>

sem_t *sem1; // the mutex semaphore
sem_t *barrier;
int *count;
int nproc;

void init_barrier(int numproc) {
    nproc = numproc;

    int shmid;
    int size = sizeof(int) + (2 * sizeof(sem_t));
    shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    count = (int *)shmat(shmid, NULL, 0);

    if (shmid == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    } else {
        printf("Shared Memory Id = %d\n", shmid);
    }

    *count = 0; // init count

    // sem1 = (sem_t *)malloc(sizeof(sem_t));
    sem1 = (sem_t *)shmat(shmid, NULL, 0);
    sem_init(sem1, 1, 1);

    // barrier = (sem_t *)malloc(sizeof(sem_t));
    barrier = (sem_t *)shmat(shmid, NULL, 0);
    sem_init(barrier, 1, 0);
}

// void reach_barrier() {
//     sem_wait(sem1);
//     // *count = *count + 1; // increment count when new process has reached barrier
//     (*count)++;
//     sem_post(sem1); // unlock the counter mutex –> other variables are free to access counter
    
//     if (*count == nproc) {
//         *count = 0; // reset count to 0
//         sem_post(barrier); // last process at the barrier sends a signal
//     } else {
//         sem_wait(barrier); // not the last process -> block cur process and wait until last process reaches
//         sem_post(barrier); // now that cur process is free, all other processes are released
//     }
// }

void reach_barrier() {
    sem_wait(sem1);
    (*count)++;
    if (*count == nproc) {
        sem_post(barrier); // last process at the barrier sends a signal
    }
    sem_post(sem1);    // unlock the counter mutex –> other variables are free to access counter
    sem_wait(barrier); // not the last process -> block cur process and wait until last process reaches
    sem_post(barrier); // now that cur process is free, all other processes are released
}

void destroy_barrier(int my_pid) {
    if (my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.

        printf("Destroying ...");

        // if parent, destroy semaphores
        sem_destroy(sem1);
        free(sem1);

        sem_destroy(barrier);
        free(barrier);

        // detach from SHM
        if (shmdt(count) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
    }
}