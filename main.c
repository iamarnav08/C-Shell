#include "input.h"

char Input[BUFFER_SIZE];
char init_home[BUFFER_SIZE];
Process* bg_process[BUFFER_SIZE];
Process* current_fg_process;
int pid_shell;
int pgid_shell;

int main(){
    current_fg_process=(Process*)malloc(sizeof(Process));
    current_fg_process->pid=-1;
    pid_shell=getpid();
    pgid_shell=getpgid(pid_shell);
    // max of buffer size aliases are allowed
    User_Function* user_functions=(User_Function*)malloc(BUFFER_SIZE*sizeof(User_Function));
    int user_function_count=0;

    load_myshrc(user_functions, &user_function_count);
    
    pid_t pid = getpid();  // Get the current process ID (the shell)
    if (setpgid(pid, pid) == -1) {
        perror("Failed to create a new process group for the shell");
        exit(EXIT_FAILURE);
    }

    if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
        perror("Failed to set the shell as the foreground process group");
        exit(EXIT_FAILURE);
    }

    printf("\033c");
    
    getcwd(init_home, BUFFER_SIZE);
    int l1=strlen(init_home);

    //making the log file
    log* LOG=init_log();
    
    load_log(LOG);

    int run=1;

    setup_signal_handler(SIGINT, ctrl_C_handler);
    setup_signal_handler(SIGQUIT, ctrl_D_handler);
    setup_signal_handler(SIGTSTP, ctrl_Z_handler);
    setup_signal_handler(SIGCHLD, background_process_handler);

    while(run){

        // Printing prompt at every turn.
        print_prompt(&l1, init_home);

        // Function to take input.
        Take_input(Input);
        if(strcmp(Input, "quit")==0){
            run=0;
            break;
        }
        if(strlen(Input)==0){
            continue;
        }
        char Input_copy[BUFFER_SIZE];
        char* rem_command;
        strncpy(Input_copy, Input, BUFFER_SIZE);
        char* command=strtok_r(Input_copy, ";", &rem_command);

        int store_check=1;
        
        while(command!=NULL){
            command[strlen(command)]='\0';
            process_command(command, &store_check, LOG, &run, user_functions, user_function_count);
            command=strtok_r(NULL, ";", &rem_command);
            
        }
        if(store_check){
                // code to store the command in the file
                write_log(Input, LOG);
                char* pwd=(char*)malloc(BUFFER_SIZE);
                getcwd(pwd, BUFFER_SIZE);
                log_hop("~");
                save_log(LOG);
                log_hop(pwd);
            }
    }
    return 0;
}
