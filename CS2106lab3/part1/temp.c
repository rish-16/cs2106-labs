// #include <semaphore.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/mman.h>
// #include <fcntl.h>
// #include <unistd.h>

// sem_t *mutex; // Semaphore to protect the count variable
// sem_t *barrier; // Semaphore to block processes at the barrier
// int *count; // Shared variable to keep track of the number of processes reaching the barrier
// int nproc; // Number of processes that need to reach the barrier

// void init_barrier(int numproc) {
//     nproc = numproc;

//     // Initializing shared memory for count
//     count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//     if (count == MAP_FAILED) {
//         perror("mmap");
//         exit(EXIT_FAILURE);
//     }
//     *count = 0;

//     // Initializing the mutex semaphore
//     mutex = sem_open("/mutex_sem", O_CREAT | O_EXCL, 0644, 1);
//     if (mutex == SEM_FAILED) {
//         perror("sem_open mutex");
//         exit(EXIT_FAILURE);
//     }

//     // Initializing the barrier semaphore
//     barrier = sem_open("/barrier_sem", O_CREAT | O_EXCL, 0644, 0);
//     if (barrier == SEM_FAILED) {
//         perror("sem_open barrier");
//         exit(EXIT_FAILURE);
//     }
// }

// void reach_barrier() {
//   sem_wait(mutex); 
//   (*count)++;
//   if (*count == nproc) {
//     sem_post(barrier);
//   }
//   sem_post(mutex);
//   sem_wait(barrier);
//   sem_post(barrier);
// }

// void destroy_barrier(int my_pid) {
//   if(my_pid != 0) { // Parent process
//     sem_close(mutex);
//     sem_unlink("/mutex_sem");
//     sem_close(barrier);
//     sem_unlink("/barrier_sem");
//     munmap(count, sizeof(int)); // Detach and free the shared memory
//   }
// }