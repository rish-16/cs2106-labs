#include <stdio.h>
#include "queue.h"

int main() {
	double v;

	for(int i = 0; i<= MAX_Q_SIZE; i++) {
		v = ((double) i / 10.0);
		printf("Adding %3.2f\n", v);
		enq(v);
	}
	
	for(int i = 0; i<= MAX_Q_SIZE; i++) {
		v = deq();
		printf("Element %d is %3.2f\n", i, v);
	}
}
