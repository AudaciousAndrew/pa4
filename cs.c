#include <stdbool.h> 

#include "ipc.h" 
#include "queue.h" 
#include "proc.h" 
#include "pa2345.h" 


/** 
* 
* @algo 
* REQUEST: 
* *) Q_i — queue of P_i; 
* When P_i needs access to CS, it sends REQUEST(L_i, i) 
* to all process and adds the message to it's own queue. 
* *) 
* 
*/ 
int 
request_cs(const void * self) { 
	process *p = (process*)self; 
	Message msg = init_msg(CS_REQUEST,0); 
	inc_time(); 
	send_multicast((void*)p, &msg); 

	push(p->queue, create_item(p->id, get_lamport_time())); 
	int wait_reply =proc_number-1; 
	while (wait_reply != 0 || (p->queue->len && p->queue->head->id != p->id) ) { 
		int id; 
		while ((id = receive_any((void*)p, &msg)) < 0); 
		set_lamport_time(msg.s_header.s_local_time);
		inc_time();
		switch (msg.s_header.s_type) { 
			case CS_REQUEST: { 
				fprintf(stderr, "%d: process %d got request from %d\n", get_lamport_time(), p->id, id); 
				push(p->queue, create_item(id, msg.s_header.s_local_time)); 
				inc_time(); 
				msg.s_header.s_type = CS_REPLY; 
				msg.s_header.s_local_time = get_lamport_time(); 
				send((void*)p, id, &msg); 
				break; 
			} 
			case CS_REPLY: { 
				fprintf(stderr, "%d: process %d got replay from %d\n", get_lamport_time(), p->id, id); 
				wait_reply--; 
				break; 
			} 
			case CS_RELEASE: { 
				fprintf(stderr, "%d: process %d got release from %d\n", get_lamport_time(), p->id, id); 
				pop(p->queue); 
				break; 
			} 
			case DONE: { 
				fprintf(stderr, "%d: process %d got DONE from %d\n", get_lamport_time(), p->id, id); 
				running_processes--; 
				break; 
			} 
			default: { 
				fprintf(stderr, "%d: process %d got unknown type: %d\n", get_lamport_time(), p->id, msg.s_header.s_type); 
			} 

		} 
	} 
	return 0; 
} 

int 
release_cs(const void * self) { 
	process *p = (process*)self; 
	Message msg = init_msg(CS_RELEASE,0); 
	inc_time(); 
	send_multicast((void*)p, &msg); 
	return 0; 
}


