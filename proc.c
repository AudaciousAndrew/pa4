#include <stdbool.h>
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
    for (size_t i = 1; i <= proc_number; i++) {
       if (i != proc->id) {
           while(receive((void*)proc, i, &tmp_msg) != 0);
           set_lamport_time(tmp_msg.s_header.s_local_time);
           inc_time();
       }
    }
}



void working_cycle(process *proc) {
    while (true) {
        Message tmp_msg = {{ 0 }};
        if (receive_any(proc, &tmp_msg) < 0) continue;
        set_lamport_time(tmp_msg.s_header.s_local_time);
        inc_time();

        if(tmp_msg.s_header.s_type == TRANSFER){
            fprintf(pipe_log,"PID %d received TRANSFER message.\n",proc->id);
            transfer_cycle(proc, balance_history, (TransferOrder*)(tmp_msg.s_payload));
        }else if(tmp_msg.s_header.s_type == STOP){
            fprintf(pipe_log,"PID %d received STOP message.\n",proc->id);
            return;
        } else {
            fprintf(pipe_log,"PID %d received wrong message with m_type %d.\n",
                proc->id, tmp_msg.s_header.s_type);
            continue;
        }
    }
}

/** Child main function. */ 
int process_c(process *proc) {

    char payload[MAX_PAYLOAD_LEN];
    size_t payload_len;

    /*ADD PROCC CREATION




    */
    


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
