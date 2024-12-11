#include "functions.h"


extern int pid_shell;
extern Process* current_fg_process;
extern int num_background_processes;

void ctrl_C_handler(int sig, siginfo_t *siginfo, void *context) {
    printf("\n");
    return;
}

void ctrl_D_handler(int sig, siginfo_t *siginfo, void *context) {
    if(sig==SIGQUIT){
        kill(0,SIGKILL);
        exit(1);
    }
}

void ctrl_Z_handler(int sig, siginfo_t *siginfo, void *context){
    int pid = getpid();
    if(pid != pid_shell){
        return;
    }
    if(current_fg_process->pid == -1){
        return;
    }
    setpgid(current_fg_process->pid, 0);
    int error=kill(current_fg_process->pid, SIGTSTP);
    if(error==-1){
        perror("Failed to stop process");
        return;
    }
    else{
        printf("\nStopped process %d\n", current_fg_process->pid);
        add_process(current_fg_process->pid, current_fg_process->command, "Stopped");
        return;
    }
    return;
}

handler* setup_signal_handler(int SigInfo, handler *SigHandler)
{
    int Error;
    struct sigaction ResultantAction, InputAction;

    memset(&ResultantAction, 0, sizeof(struct sigaction));
    ResultantAction.sa_sigaction = SigHandler;
    sigemptyset(&ResultantAction.sa_mask);

    ResultantAction.sa_flags = SA_RESTART | SA_SIGINFO;
    sigaction(SigInfo, &ResultantAction, &InputAction);

    Error = sigaction(SigInfo, &ResultantAction, &InputAction);

    if (Error == -1)
        write(1, "Error - Signal Handler: Initialization error\n", strlen("Error - Signal Handler: Initialization error\n"));
    return (InputAction.sa_sigaction);
}

void ping_process(pid_t pid, int signal_number) {
    // Calculate signal number modulo 32
    int mod_signal = signal_number % 32;

    // Check if the signal number is valid
    if (mod_signal <= 0 || mod_signal >= NSIG) {
        fprintf(stderr, "Error: Invalid signal number\n");
        return;
    }

    // Check if the process with the given PID exists
    if (kill(pid, 0) == -1) {
        if (errno == ESRCH) {
            printf("No such process found\n");
        } else {
            perror("Error checking process");
        }
        return;
    }

    // Send the signal to the process
    if (kill(pid, mod_signal) == -1) {
        perror("Error sending signal");
    } else {
        printf("Signal %d sent to process %d\n", mod_signal, pid);
        if(mod_signal==SIGKILL || mod_signal == SIGTERM || mod_signal==SIGINT){
            remove_process(pid);
        }
        else if(mod_signal==SIGTSTP || mod_signal==SIGSTOP){
            update_process_state(pid, "Stopped");
        }
        else if(mod_signal==SIGCONT){
            update_process_state(pid, "Running");
        }
    }
    list_activities();
}

