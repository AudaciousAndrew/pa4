#include <stdio.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 

#include "ipc.h" 
#include "queue.h" 

list_item *create_item(local_id id, timestamp_t time) { 
	list_item *item = (list_item*)malloc(sizeof(list_item)); 
	if (item == NULL) { 
		perror("malloc"); 
		exit(-1); 
	} 
	item->next = NULL; 
	item->id = id; 
	item->time = time; 
	return item; 
} 

queue *init(void) { 
	queue *q = (queue*)malloc(sizeof(queue)); 
	if (q == NULL) { 
		perror("malloc"); 
		exit(-1); 
	} 
	q->len = 0; 
	q->head = NULL; 
	return q; 
} 

void destroy(queue *q) { 
	list_item *item; 
	while (q->head) { 
		item = q->head->next; 
		free(q->head); 
		q->head = item; 
	} 
	free(q); 
} 

void push(queue *q, list_item *n) { 
	list_item *cur = NULL; 
	list_item *prev = NULL; 
	size_t len = q->len; 
	if (q->head == NULL) { 
		q->head = n; 
		return; 
	} 

	for (cur = q->head; cur; prev = cur, cur = cur->next) { 
		if (cur->time > n->time || (cur->time == n->time && cur->id > n->id)) { 
			n->next = cur; 
			if (prev) 
				prev->next = n; 
			else if (cur == q->head) 
				q->head = n; 

			q->len++; 
			break; 
		} 
	} 
	if (len == q->len) { 
	prev->next = n; 
	} 
} 

void pop(queue *q) { 
	list_item *next = q->head->next; 
	free(q->head); 
	q->head = next; 
} 

void print_queue(queue *q, int id) { 
	list_item *t = q->head; 
	for (int i = 0; t; t = t->next, i++) { 
		fprintf(stderr, "Process %d #%d: ID[%d] TIME[%d]\n", id, i, t->id, t->time); 
	} 
} 

