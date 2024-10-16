#pragma once

/*
`mpsc` stands for "multiple producer, single consumer". This queue is threadsafe provided it is used
in a "single-consumer, multiple producer" manner. i.e. `mpsc_queue_enqueue` can be called by many
different threads, but `mpsc_queue_dequeue` must only be called from a single thread.

Note that the `mpsc_queue` expects the events that it contains to have been individually allocated
on the heap, and it takes ownership of them, freeing any remaining events on the queue when the
`mpsc_queue` itself is destroyed. When dequeueing events, you take on responsibility for freeing the
events.
*/
typedef struct mpsc_queue mpsc_queue;

mpsc_queue *mpsc_queue_create();

void mpsc_queue_destroy(mpsc_queue *q);

void mpsc_queue_enqueue(mpsc_queue *q, void *evt);

void *mpsc_queue_dequeue(mpsc_queue *q);
