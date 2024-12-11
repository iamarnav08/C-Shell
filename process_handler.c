#include "functions.h"


extern Process* bg_process[BUFFER_SIZE];
extern int num_background_processes;

void init_background_processes() {
    for (int i = 0; i < max_num_background_processes; i++) {
        bg_process[i] = (Process *)malloc(sizeof(Process));
        strcpy(bg_process[i]->command, "");
        bg_process[i]->pid = -1;
    }
}

void kill_all_processes() {
    for (int i = 0; i < num_background_processes; i++) {
        if (bg_process[i] != NULL && bg_process[i]->pid > 0) {
            if (kill(bg_process[i]->pid, SIGTERM) == -1) {
                perror("Failed to terminate background process");
            } else {
                printf("Terminated process %d\n", bg_process[i]->pid);
            }
            free(bg_process[i]);
            bg_process[i] = NULL;
        }
    }
    num_background_processes = 0;
}

void background_process_handler(int Sig, siginfo_t *Info, void *Pointer)
{
    int Status, PID, Check = 0, ExitStatus;
    char buffer[4200];

    // Look for any background process that just ended
    while ((PID = waitpid(-1, &Status, WNOHANG | WUNTRACED)) > 0)
    {
        for (int i = 0; i < num_background_processes; i++)
        {
            // Compare the PID of the process that ended to the processes stored in the array till we find which it was
            if (bg_process[i]->pid == PID)
            {
                // If we find the process, we check its exit status
                Check = 1;
                ExitStatus = WIFEXITED(Status);
                if (ExitStatus == 0)
                    snprintf(buffer,sizeof(buffer), "\n%s with pid [%d] exited abnormally\n", bg_process[i]->command, bg_process[i]->pid);
                else
                    snprintf(buffer,sizeof(buffer), "\n%s with pid [%d] exited normally\n", bg_process[i]->command, bg_process[i]->pid);
                write(1, buffer, strlen(buffer));

                // We also check if it has terminated or if it has just been stopped
                if (!WIFSTOPPED(Status))
                    bg_process[i]->pid = -1;
                break;
            }
        }
    }
}


void remove_process(pid_t pid) {
    for (int i = 0; i < num_background_processes; i++) {
        if (bg_process[i] != NULL && bg_process[i]->pid == pid) {
            // Free the memory allocated for the process
            free(bg_process[i]);

            // Shift subsequent processes left to fill the gap
            for (int j = i; j < num_background_processes - 1; j++) {
                bg_process[j] = bg_process[j + 1];
            }

            // Clear the last slot
            bg_process[num_background_processes - 1] = NULL;

            // Update the count of background processes
            num_background_processes--;

            break; // Exit the loop once the process is found and removed
        }
    }
}


void add_process(int pid,  char *command,  char *state) {
    if(num_background_processes<max_num_background_processes){
        bg_process[num_background_processes]= (Process *)malloc(sizeof(Process));
        bg_process[num_background_processes]->pid = pid;
        strcpy(bg_process[num_background_processes]->command, command);
        strcpy(bg_process[num_background_processes]->status, state);
        num_background_processes++;
        printf("Process added\n");
    }
    else{
        printf("Max number of background processes reached\n");
    }
    return;
}

void update_process_state(int pid, char* state) {
    for (int i = 0; i < num_background_processes; i++) {
        if (bg_process[i] != NULL && bg_process[i]->pid == pid) {
            strcpy(bg_process[i]->status, state);
            break;
        }
    }
}
