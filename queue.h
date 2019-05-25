#ifndef __IFMO_DISTCOMP_QUEUE_H__ 
#define __IFMO_DISTCOMP_QUEUE_H__ 

typedef struct list_item { 
	struct list_item *next; 
	local_id pid; 
	timestamp_t time; 
} list_item; 

typedef struct queue { 
	list_item *head; 
} queue; 

list_item *create_item(local_id id, timestamp_t time); 

queue *init(void); 
void destroy(queue *q); 
void pop(queue *q); 
void push(queue *queue, list_item *n); 


#endif /* __IFMO_DISTCOMP_QUEUE_H__ */
