#pragma once

#include <stdbool.h>

/*
`mpsc` stands for "multiple producer, single consumer". This queue is threadsafe
provided it is used in a "single-consumer, multiple producer" manner. i.e.
`mpsc_queue_enqueue` can be called by many different threads, but
`mpsc_queue_dequeue` must only be called from a single thread.

Note that the `mpsc_queue` expects the events that it contains to have been
individually allocated on the heap, and it takes ownership of them, freeing them
when the `mpsc_queue` itself is destroyed.
*/
typedef struct mpsc_queue mpsc_queue;

mpsc_queue *mpsc_queue_create(void);

void mpsc_queue_destroy(mpsc_queue *q);

void mpsc_queue_enqueue(mpsc_queue *q, void *events);

void *mpsc_queue_dequeue(mpsc_queue *q);
