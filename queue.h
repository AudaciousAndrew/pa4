#ifndef __IFMO_DISTCOMP_QUEUE_H__ 
#define __IFMO_DISTCOMP_QUEUE_H__ 

typedef struct list_item { 
	struct list_item *next; 
	local_id id; 
	timestamp_t time; 
} list_item; 

typedef struct queue { 
	list_item *head; 
	size_t len; 
} queue; 

list_item *create_item(local_id id, timestamp_t time); 

queue *init(void); 
void destroy(queue *q); 
void pop(queue *q); 
void push(queue *q, list_item *n); 
void print_queue(queue *q, int id); 


#endif /* __IFMO_DISTCOMP_QUEUE_H__ */
