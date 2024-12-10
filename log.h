#ifndef LOG_H
#define LOG_H

#include "functions.h"

#define LOG_FILE ".log_history"
#define LOG_FILE_SIZE 15 

typedef struct log{
    char command_log[LOG_FILE_SIZE][BUFFER_SIZE];
    int size;
    int log_index;
    int count;
}log;

log* init_log();
void load_log(log* LOG);    // loads log file into the struct at start of the shell
void save_log(log* LOG);    // saves the log file into the file at the end of the shell
void write_log(char* command, log* LOG);    // writes the command into the log file
void print_log(log* LOG);   // prints the log file
void purge_log(log* LOG);
void execute_log(log* LOG, int index);

void process_execute(char* command);
void execute_command_log(char* command, int background);

void log_hop(char* path);

#endif