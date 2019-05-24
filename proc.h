#include <stdio.h>
#include "ipc.h"
#include "banking.h"
#include "common.h"
#include "pa2345.h"

#define ARR_SIZE    11
#define FD_MAX      2
#define READ_FD     0
#define WRITE_FD    1

#define ARGUMENTS_OFFSET 3

int proc_number;
int pipes[ARR_SIZE][ARR_SIZE][FD_MAX];
int mutexl;
timestamp_t lamport_time;
FILE *event_log;
FILE *pipe_log;

int init_pipes(int pipes[ARR_SIZE][ARR_SIZE][FD_MAX]);
void close_fds(int pipes[ARR_SIZE][ARR_SIZE][FD_MAX], local_id id);

typedef struct {
    local_id id;   /**< ID for the process. */
} process;

int get_arguments(int argc, char **argv);
Message init_msg(MessageType type , size_t payload_len);
void receive_all_msg(process *proc, MessageType m_type);

int process_c(process *proc);
void synchronize(process *proc, MessageType m_type, char *payload, size_t payload_len);
void working_cycle(process *proc);

/* lamport */
timestamp_t get_lamport_time();
void set_lamport_time(timestamp_t time);
void inc_time();
