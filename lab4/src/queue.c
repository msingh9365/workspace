#include "common.h"
#include <stdlib.h>

int ts_queue_init(ts_queue_t *q, size_t capacity) {
	q->items = (void **)calloc(capacity, sizeof(void *));
	if (!q->items) return -1;
	q->capacity = capacity;
	q->head = 0;
	q->tail = 0;
	q->count = 0;
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->not_empty, NULL);
	pthread_cond_init(&q->not_full, NULL);
	return 0;
}

void ts_queue_destroy(ts_queue_t *q) {
	free(q->items);
	pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->not_empty);
	pthread_cond_destroy(&q->not_full);
}

int ts_queue_put(ts_queue_t *q, void *item) {
	pthread_mutex_lock(&q->mutex);
	while (q->count == q->capacity) {
		pthread_cond_wait(&q->not_full, &q->mutex);
	}
	q->items[q->tail] = item;
	q->tail = (q->tail + 1) % q->capacity;
	q->count++;
	pthread_cond_signal(&q->not_empty);
	pthread_mutex_unlock(&q->mutex);
	return 0;
}

void *ts_queue_take(ts_queue_t *q) {
	pthread_mutex_lock(&q->mutex);
	while (q->count == 0) {
		pthread_cond_wait(&q->not_empty, &q->mutex);
	}
	void *item = q->items[q->head];
	q->head = (q->head + 1) % q->capacity;
	q->count--;
	pthread_cond_signal(&q->not_full);
	pthread_mutex_unlock(&q->mutex);
	return item;
}