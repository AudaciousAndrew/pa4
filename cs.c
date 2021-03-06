#include <stdbool.h> 

#include "ipc.h" 
#include "queue.h" 
#include "proc.h" 


int 
request_cs(const void * self) { 
	process *procc = (process*)self; 
	Message tmp_msg = init_msg(CS_REQUEST,0); 
	int16_t header_type = tmp_msg.s_header.s_type;
	timestamp_t local_time = tmp_msg.s_header.s_local_time;

	inc_time(); 
	send_multicast((void*)procc, &tmp_msg); 


	push(procc->queue, create_item(procc->id, get_lamport_time())); 
	int w_reply = proc_number-1; 
	while (w_reply != 0 ) { 
		int id; 
		while ((id = receive_any((void*)procc, &tmp_msg)) < 0); 
		set_lamport_time(local_time);
		inc_time();
		if(header_type == CS_REQUEST){
			fprintf(stderr, "%d: PID %d received CS_REQUEST from %d\n", get_lamport_time(), procc->id, id); 
			push(procc->queue, create_item(id, local_time)); 
			inc_time(); 
			tmp_msg = init_msg(CS_REPLY,0);
			send((void*)procc, id, &tmp_msg); 
		}else  if(header_type == CS_REPLY) {
			fprintf(stderr, "%d: PID %d received CS_REPLY from %d\n", get_lamport_time(), procc->id, id); 
			w_reply--; 
		}else if(header_type == CS_RELEASE){
			fprintf(stderr, "%d: PID %d received CS_RELEASE from %d\n", get_lamport_time(), procc->id, id); 
			pop(procc->queue); 
		}else if(header_type == DONE){
			fprintf(stderr, "%d: PID %d received DONE from %d\n", get_lamport_time(), procc->id, id); 
			running_processes--; 
		}else {
			fprintf(stderr, "%d: PID %d received UNKNOWN m_type: %d\n", get_lamport_time(), procc->id, header_type); 
		}
	} 
	return 0; 
} 

int 
release_cs(const void * proc) { 
	Message tmp_msg = init_msg(CS_RELEASE,0); 
	inc_time(); 
	send_multicast((void*)proc, &tmp_msg); 
	return 0; 
}


