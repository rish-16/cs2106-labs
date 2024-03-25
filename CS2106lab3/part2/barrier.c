#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/shm.h>

// sem_t *sem1; // the mutex semaphore
// sem_t *barrier;
// int *count;
// int nproc;

typedef struct {
    sem_t *sem1; // the mutex semaphore
    sem_t *barrier;
    int *count;
    int nproc;
} BarrierObject;

BarrierObject *barrier_object;

void init_barrier(int numproc) {
    // nproc = numproc;
    int shmid;

    int size = sizeof(int) + (2 * sizeof(sem_t));
    shmid = shmget(IPC_PRIVATE, sizeof(BarrierObject), IPC_CREAT | 0666);

    if (shmid == -1) {
        printf("Cannot create shared memory!\n");
        exit(1);
    } else {
        printf("Shared Memory Id = %d\n", shmid);
    }

    barrier_object = (BarrierObject *)shmat(shmid, NULL, 0);
    barrier_object->nproc = numproc;
    barrier_object->count = 0;

    sem_init(barrier_object->sem1, 1, 1);
    sem_init(barrier_object->barrier, 1, 0);
    printf("CREATED")
}

// void reach_barrier() {
//     sem_wait(sem1);
//     // *count = *count + 1; // increment count when new process has reached barrier
//     (*count)++;
//     sem_po*st(sem1); // unlock the counter mutex –> other variables are free to access counter
    
//     if (*count == nproc) {
//         *count = 0; // reset count to 0
//         sem_post(barrier); // last process at the barrier sends a signal
//     } else {
//         sem_wait(barrier); // not the last process -> block cur process and wait until last process reaches
//         sem_post(barrier); // now that cur process is free, all other processes are released
//     }
// }

void reach_barrier() {
    sem_wait(barrier_object->sem1);
    (barrier_object->count)++;
    sem_post(barrier_object->sem1); // unlock the counter mutex –> other variables are free to access counter
    if (barrier_object->count == barrier_object->nproc) {
        sem_post(barrier_object->barrier); // last process at the barrier sends a signal
    } else {
        sem_wait(barrier_object->barrier); // not the last process -> block cur process and wait until last process reaches
        sem_post(barrier_object->barrier); // now that cur process is free, all other processes are released
    }
}

void destroy_barrier(int my_pid) {
    if (my_pid != 0) {
        // Destroy the semaphores and detach
        // and free any shared memory. Notice
        // that we explicity check that it is
        // the parent doing it.

        printf("Destroying ...");

        // if parent, destroy semaphores
        sem_destroy(barrier_object->sem1);
        free(barrier_object->sem1);

        sem_destroy(barrier_object->barrier);
        free(barrier_object->barrier);

        // detach from SHM
        if (shmdt(barrier_object->count) == -1) {
            perror("shmdt");
            exit(EXIT_FAILURE);
        }

        if (shmdt(barrier_object) == -1){
            perror("shmdt");
            exit(EXIT_FAILURE);
        }
    }
}