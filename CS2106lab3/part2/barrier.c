// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <semaphore.h>
// #include <sys/wait.h>
// #include <sys/shm.h>

// int *count;
// int nproc;
// sem_t *sem1; // the mutex semaphore
// sem_t *barrier;

// void init_barrier(int numproc) {
//     nproc = numproc;

//     int shmid;
//     shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
//     count = (int *)shmat(shmid, NULL, 0);

//     if (shmid == -1) {
//         printf("Cannot create shared memory!\n");
//         exit(1);
//     } else {
//         printf("Shared Memory Id = %d\n", shmid);
//     }

//     *count = 0; // init count

//     sem1 = (sem_t *)malloc(sizeof(sem_t));
//     sem_init(sem1, 1, 1);

//     barrier = (sem_t *)malloc(sizeof(sem_t));
//     sem_init(barrier, 1, 0);
// }

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

// void destroy_barrier(int my_pid) {
//     if(my_pid != 0) {
//         // Destroy the semaphores and detach
//         // and free any shared memory. Notice
//         // that we explicity check that it is
//         // the parent doing it.

//         // if parent, destroy semaphores
//         sem_destroy(sem1);
//         // free(sem1);

//         sem_destroy(barrier);
//         // free(barrier);

//         // detach from SHM
//         if (shmdt(count) == -1) {
//             perror("shmdt");
//             exit(EXIT_FAILURE);
//         }
//     }
// }

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

sem_t *mutex;   // Semaphore to protect the count variable
sem_t *barrier; // Semaphore to block processes at the barrier
int *count;     // Shared variable to keep track of the number of processes reaching the barrier
int nproc;      // Number of processes that need to reach the barrier

void init_barrier(int numproc)
{
    nproc = numproc;

    // Initializing shared memory for count
    count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (count == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    *count = 0;

    // Initializing the mutex semaphore
    mutex = sem_open("/mutex_sem", O_CREAT | O_EXCL, 0644, 1);
    if (mutex == SEM_FAILED)
    {
        perror("sem_open mutex");
        exit(EXIT_FAILURE);
    }

    // Initializing the barrier semaphore
    barrier = sem_open("/barrier_sem", O_CREAT | O_EXCL, 0644, 0);
    if (barrier == SEM_FAILED)
    {
        perror("sem_open barrier");
        exit(EXIT_FAILURE);
    }
}

void reach_barrier()
{
    sem_wait(mutex);
    (*count)++;
    if (*count == nproc)
    {
        sem_post(barrier);
    }
    sem_post(mutex);
    sem_wait(barrier);
    sem_post(barrier);
}

void destroy_barrier(int my_pid)
{
    if (my_pid != 0)
    { // Parent process
        sem_close(mutex);
        sem_unlink("/mutex_sem");
        sem_close(barrier);
        sem_unlink("/barrier_sem");
        munmap(count, sizeof(int)); // Detach and free the shared memory
    }
}