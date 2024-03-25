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

    int shmid1, shmid2, shmid3;
    shmid1 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    shmid2 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    shmid3 = shmget(IPC_PRIVATE, sizeof(sem_t), IPC_CREAT | 0666);
    if (shmid1 == -1 || shmid2 == -1 || shmid3 == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    } else {
        printf("Shared Memory Id = %d\n", shmid1);
        printf("Shared Memory Id = %d\n", shmid2);
        printf("Shared Memory Id = %d\n", shmid3);
    }

    count = (int *) shmat(shmid1, NULL, 0);
    *count = 0;

    sem1 = (sem_t *) shmat(shmid2, NULL, 0);
    barrier = (sem_t *) shmat(shmid3, NULL, 0);
    sem_init(sem1, 1, 1);
    sem_init(barrier, 1, 0);
}

void reach_barrier() {
    sem_wait(sem1);
    *count = *count + 1; // increment count when new process has reached barrier
    sem_post(sem1); // unlock the counter mutex –> other variables are free to access counter
    
    if (*count == nproc) {
        *count = 0; // reset count to 0
        sem_post(barrier); // last process at the barrier sends a signal
    } else {
        sem_wait(barrier); // not the last process -> block cur process and wait until last process reaches
        sem_post(barrier); // now that cur process is free, all other processes are released
    }
}

void destroy_barrier(int my_pid) {
    if (my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.

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

        if (shmdt(sem1) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }

        if (shmdt(barrier) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
    }
}