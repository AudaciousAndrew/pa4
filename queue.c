#include <stdio.h> 
#include <errno.h> 
#include <unistd.h> 
#include <stdlib.h> 

#include "ipc.h" 
#include "queue.h" 

list_item *create_item(local_id id, timestamp_t time) { 
	list_item *item = (list_item*)malloc(sizeof(list_item)); 
	item->next = NULL; 
	item->pid = id; 
	item->time = time; 
	return item; 
} 

queue *init(void) { 
	queue *queuE = (queue*)malloc(sizeof(queue)); 
	queuE->head = NULL; 
	return queuE; 
} 

void destroy(queue *queue) { 
	list_item *item; 
	while (queue->head != NULL) { 
		item = queue->head->next; 
		free(queue->head); 
		queue->head = item; 
	} 
	free(queue); 
} 

void pop(queue *queue) { 
	list_item *next = queue->head->next; 
	free(queue->head); 
	queue->head = next; 
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

