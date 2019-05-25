#include <stdio.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 

#include "ipc.h" 
#include "queue.h" 

list_item *create_item(local_id id, timestamp_t time) { 
	list_item *item = (list_item*)malloc(sizeof(list_item)); 
	// if (item == NULL) { 
	// 	perror("malloc"); 
	// 	exit(-1); 
	// } 
	item->next = NULL; 
	item->pid = id; 
	item->time = time; 
	return item; 
} 

queue *init(void) { 
	queue *q = (queue*)malloc(sizeof(queue)); 
	// if (q == NULL) { 
	// 	perror("malloc"); 
	// 	exit(-1); 
	// } 
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

void push(queue *queue, list_item *new_item) { 
	list_item *current = NULL; 
	list_item *prev = NULL; 
	if (queue->head == NULL) { 
		queue->head = new_item; 
		return; 
	} 
	current = queue->head;
	while(current != NULL) {
		if (current->time > new_item->time ||
		 (current->time == new_item->time && current->pid > new_item->pid)) { 
			new_item->next = current;
			if(prev) prev->next = new_item;
			if(current == queue->head) queue->head = new_item;
			new_item = NULL;
			break;
		} else {
			prev = current;
			current = current->next;
		}
	}

	if(new_item) {
		prev->next = new_item;
		new_item = NULL;
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
		fprintf(stderr, "Process %d #%d: ID[%d] TIME[%d]\n", id, i, t->pid, t->time); 
	} 
} 

