#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define BUFFER_SIZE 4096
#define max_num_background_processes 10

extern char prev_dir[BUFFER_SIZE];
extern int foreground_pid;

// ------------------ Structures ------------------
typedef struct prompt_strings{
    char* user_name;
    char* host_name;
    char* directory;
}prompt_strings;

typedef struct{
    char alias[BUFFER_SIZE];
    char actual_command[BUFFER_SIZE];
}User_Function;

typedef struct Process {
    int pid;
    char command[BUFFER_SIZE];
    char status[BUFFER_SIZE];
    struct Process* next;
}Process;

// ------------------ Function Prototypes ------------------

//------------------Functions for signal and ping handling-----------
typedef void handler(int sig, siginfo_t *info, void *vp);
handler* setup_signal_handler(int SigInfo, handler *SigHandler);
void ctrl_C_handler(int sig, siginfo_t *siginfo, void *context);
void ctrl_Z_handler(int sig, siginfo_t *siginfo, void *context);
void ctrl_D_handler(int sig, siginfo_t *siginfo, void *context);
void ping_process(pid_t pid, int signal_number);

//------------------Functions for process handling-----------
void background_process_handler(int Sig, siginfo_t *Info, void *Pointer);
void init_background_processes();
void add_process(int pid,  char* command,  char* state);
void remove_process(pid_t pid);
void update_process_state(int pid, char* state);

//------------------Functions for prompts-----------
prompt_strings* get_prompt_strings();
void print_prompt(int* home_dir_len, char* home_dir);


//------------------User-made Functions-----------
void list_activities();
void fg(int pid);
void bg(int pid);
void load_myshrc(User_Function* user_functions, int* user_function_count);
void enable_raw_mode(struct termios *orig_termios);
void disable_raw_mode(struct termios *orig_termios);
void neonate(int time_arg);


#endif

