
#define MAX_FNAME_LEN   64

typedef struct llist {
    char filename[MAX_FNAME_LEN];
    int filesize;
    int startblock;
    struct llist *prev, *next;
} TLinkedList;

// Fill in the prototypes below

