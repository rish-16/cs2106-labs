#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "config.h"
#include "barrier.h"

#define NUM_PROCESSES  8

int main() {

    int vect[VECT_SIZE];
    int pid;
    int shmid1;
    long int *all_sum;
    clock_t start, end;
    double time_taken;

    float per_process_raw = (float) VECT_SIZE / NUM_PROCESSES;
    int per_process = (int) per_process_raw;

    if(per_process_raw != (float) per_process) {
        printf("Vector size of %d is not divisible by %d processes.\n", VECT_SIZE, NUM_PROCESSES);
        exit(-1);
    }

    int i;

    srand(24601);
    for(i=0; i<VECT_SIZE; i++)
        vect[i] = rand();

    shmid1 = shmget(IPC_PRIVATE, NUM_PROCESSES * sizeof(int), IPC_CREAT | 0600);
    all_sum = (long int *) shmat(shmid1, NULL, 0);

    for(i=0; i<NUM_PROCESSES; i++) {
        pid = fork();

        if(pid == 0)
            break;
    }

    int j;
    long int sum = 0; // local sum

    init_barrier(NUM_PROCESSES+1);

    if(pid == 0) {
	    /*insert code */
        // child

        /*
        p0 : [0, 250000)
        p1 : [250000, 500000)
        p2 : [500000, 750000)
        p3 : [750000, 1000000)
        p4 : [1000000, 1250000)
        p5 : [1250000, 1500000)
        p6 : [1500000, 1750000)
        p7 : [1750000, 2000000)
        */

        long int local_sum = 0;
        int range_table[NUM_PROCESSES][2] = {{0, 250000}, {250000, 500000}, {500000, 750000}, {750000, 1000000}, {1000000, 1250000}, {1250000, 1500000}, {1500000, 1750000}, {1750000, 2000000}};
        // int range[2] = range_table[i];
        // int lo = range[0];
        // int hi = range[1];

        // sum the elements in that subset
        for (int k = range_table[i][0]; k < range_table[i][1]; k++) {
            local_sum += vect[k];
        }

        all_sum[i] = local_sum; // store local sum
        reach_barrier(); // wait for other processes to reach the end
    } else {
        start = clock();
    
   	    /* insert code */
        // parent -> add up all the local sums

        reach_barrier(); // check if all processes have reached barrier, if not parent waits
        
        // sum up all N local sums
        for (j = 0; j < NUM_PROCESSES; j++) {
            sum += all_sum[j];
        }

        // parent has finished adding local sums
    
        end = clock();

        time_taken = ((double) end - start) / CLOCKS_PER_SEC;

        printf("\nNumber of items: %d\n", VECT_SIZE);
        printf("Sum element is %ld\n", sum);
        printf("Time taken is %3.10f\n\n", time_taken);

        // Clean up process table
        for(j=0; j<NUM_PROCESSES; j++) {
            wait(NULL);
        }

        destroy_barrier(pid);

        shmdt(all_sum);
        shmctl(shmid1, IPC_RMID, 0);
    }
}



