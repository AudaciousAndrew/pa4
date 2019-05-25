#include <stdbool.h> 

#include "ipc.h" 
#include "queue.h" 
#include "proc.h" 


int 
request_cs(const void * self) { 
	process *p = (process*)self; 
	Message msg = init_msg(CS_REQUEST,0); 
	inc_time(); 
	send_multicast((void*)p, &msg); 

	push(p->queue, create_item(p->id, get_lamport_time())); 
	int wait_reply =proc_number-1; 
	while (wait_reply != 0 /*|| (p->queue->len && p->queue->head->pid != p->id) */) { 
		int id; 
		while ((id = receive_any((void*)p, &msg)) < 0); 
		set_lamport_time(msg.s_header.s_local_time);
		inc_time();
		if(msg.s_header.s_type == CS_REQUEST){
			fprintf(stderr, "%d: PID %d received CS_REQUEST from %d\n", get_lamport_time(), p->id, id); 
			push(p->queue, create_item(id, msg.s_header.s_local_time)); 
			inc_time(); 
			msg.s_header.s_type = CS_REPLY; 
			msg.s_header.s_local_time = get_lamport_time(); 
			send((void*)p, id, &msg); 
		}else  if(msg.s_header.s_type == CS_REPLY) {
			fprintf(stderr, "%d: PID %d received CS_REPLY from %d\n", get_lamport_time(), p->id, id); 
			wait_reply--; 
		}else if(msg.s_header.s_type == CS_RELEASE){
			fprintf(stderr, "%d: PID %d received CS_RELEASE from %d\n", get_lamport_time(), p->id, id); 
			pop(p->queue); 
		}else if(msg.s_header.s_type == DONE){
			fprintf(stderr, "%d: PID %d received DONE from %d\n", get_lamport_time(), p->id, id); 
			running_processes--; 
		}else {
			fprintf(stderr, "%d: PID %d received UNKNOWN m_type: %d\n", get_lamport_time(), p->id, msg.s_header.s_type); 
		}
	} 
	return 0; 
} 

int 
release_cs(const void * proc) { 
	Message msg = init_msg(CS_RELEASE,0); 
	inc_time(); 
	send_multicast((void*)proc, &msg); 
	return 0; 
}


