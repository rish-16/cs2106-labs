#include "queue.h"
#include <stdio.h>

static double  _queue[MAX_Q_SIZE];
static int _front = 0, _rear = 0;

void enq(double  data) {
	if ((_front + 1) % MAX_Q_SIZE == _rear){
		printf("Error: Queue is full. Item value %3.2f is not added.\n", data);
	}
	else {
		_queue[_front] = data;
		_front = (_front + 1) % MAX_Q_SIZE;
	}
}

double deq() {
	double val = -1;
	if (_rear == _front){
		printf("Error: Queue is empty. Nothing to return\n");
	}
	else {
		val = _queue[_rear];
		_rear = (_rear + 1) % MAX_Q_SIZE;
	}

	return val;
}

/* This section is for the function pointers exercise */

static double _res;

double sum(double x, double y) {
    return x + y;
}

double prod(double x, double y) {
    return x * y;
}

void clear_sum(double *acc) {
    *acc = 0.0;
}

void clear_prod(double *acc) {
    *acc = 1.0;
}

double reduce() {
    int ndx = _rear;

    clear_sum(&_res);
    while(ndx != _front) {
        _res = sum(_queue[ndx], _res);
        ndx = (ndx + 1) % MAX_Q_SIZE;
    }
    return _res;
}

double flex_reduce(void (*clear)(double *), double (*op)(double, double)) {
    int ndx = _rear;

    clear(&_res);
    while(ndx != _front) {
        _res = op(_queue[ndx], _res);
        ndx = (ndx + 1) % MAX_Q_SIZE;
    }
    return _res;
}

