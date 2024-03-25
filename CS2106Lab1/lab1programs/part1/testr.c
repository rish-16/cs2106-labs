#include <stdio.h>
#include "queue.h"

int main() {

    int i;
    for(i=1; i<=9; i++)
    {
        printf("Enqueing %d\n", i);
        enq((double) i);
    }

    printf("\nCalling reduce result is %3.2f\n", reduce());

    /* Uncomment the following two statements to test flex_reduce */

    printf("Calling flex reduce with sum. Result is %3.2f\n", flex_reduce(clear_sum, sum));
    printf("Calling flex reduce with prod. Result is %3.2f\n", flex_reduce(clear_prod, prod));
}

