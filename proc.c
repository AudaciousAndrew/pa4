#define _DEFAULT_SOURCE
#include <stdbool.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


#include "proc.h"



/** Syncronization cycle. */
void synchronize(process *proc, MessageType m_type, char *payload, size_t payload_len) {
    inc_time();
    Message tmp_msg = init_msg(m_type , payload_len);
    memcpy(tmp_msg.s_payload, payload, payload_len);
    send_multicast((void*)proc, (const Message *)&tmp_msg);
    /* new part */
    int total = proc_number - 1;
    while (total > 0) {
        while (receive_any((void*)proc, &tmp_msg) < 0);
        set_lamport_time(tmp_msg.s_header.s_local_time);
           inc_time();
        if(m_type == tmp_msg.s_header.s_type) --total; 
    }
}



void working_cycle(process *proc) {

    if (mutexl)
        request_cs((void*)proc);
    
    for (int i = 1; i <= 5 * proc->id; i++) {
        size_t slen = snprintf(NULL, 0, log_loop_operation_fmt, proc->id, i,
                               5 * proc->id);
        char *str = (char*)alloca(slen + 1);
        snprintf(str, slen+1, log_loop_operation_fmt, proc->id, i, 5 * proc->id);
        print(str);
    }

    if (mutexl)
        release_cs((void*)proc);
}

/** Child main function. */ 
int process_c(process *proc) {

    char payload[MAX_PAYLOAD_LEN];
    size_t payload_len; 

    proc->queue = init();


    close_fds(pipes, proc->id);
    /* Process starts. */

    payload_len = sprintf(payload, log_started_fmt, 
                  get_lamport_time(), proc->id, getpid(),
                  getppid(), 0);
    fputs(payload, event_log); 

    /* Synchronization */
    synchronize(proc, STARTED, payload, payload_len);
    fprintf(event_log, log_received_all_started_fmt,
            get_lamport_time(), proc->id);

    /* Work. */
    working_cycle(proc);

    /* Process's done. */
    payload_len = sprintf(payload, log_done_fmt, get_lamport_time(), 
            proc->id, 0);
    fputs(payload, event_log); 

    /* Synchronization */
    synchronize(proc, DONE, payload, payload_len);
    fprintf(event_log, log_received_all_done_fmt, get_lamport_time(), proc->id);

    return 0;
}
