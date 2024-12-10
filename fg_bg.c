#include "specific_commands.h"
#include "headers.h"
#include "functions.h"
#include "input.h"

extern char init_home[];
char prev_dir[BUFFER_SIZE]="";
extern Process* bg_process[BUFFER_SIZE];
extern int num_background_processes;
extern Process* current_fg_process;

void fg(int pid) {
    int found = 0;  // Flag to check if the process is found
    for (int i = 0; i < num_background_processes; i++) {
        if (bg_process[i] != NULL && bg_process[i]->pid == pid) {
            found = 1;  // Process found
            // Check if the process is stopped or running
            printf("Process ID: %d\n", pid);
            printf("Command: %s\n", bg_process[i]->command);
            printf("Status: %s\n", bg_process[i]->status);
            if (strcmp(bg_process[i]->status, "Stopped") == 0 || strcmp(bg_process[i]->status, "Running") == 0) {
                // Send SIGCONT signal to continue the process
                // kill(pid, SIGCONT);
                if(kill(pid, SIGCONT)==0){
                    strcpy(bg_process[i]->status, "Running");
                    printf("Process %d: %s - now Running in the foreground\n", pid, bg_process[i]->command);
                    current_fg_process->pid = pid;
                    strcpy(current_fg_process->command, bg_process[i]->command);
                    // Wait for the process to finish
                    int status;
                    waitpid(pid, &status, WUNTRACED);
                    current_fg_process->pid = -1;
                    bg_process[i]->pid = -1;
                    strcpy(bg_process[i]->status, "Completed");
                } else {
                    perror("Error sending SIGCONT");
                }
            }

            // After the process has finished, remove it from the list (optional)
            
            break;
        }
    }

    if (!found) {
        printf("No such process found\n");
    }
}


void bg(int pid) {
    int found = 0;  // Flag to check if the process is found
    for (int i = 0; i < num_background_processes; i++) {
        if (bg_process[i] != NULL && bg_process[i]->pid == pid) {
            found = 1;  // Process found
            // Check if the process is stopped
            if (strcmp(bg_process[i]->status, " Stopped") == 0) {
                // Send SIGCONT signal to continue the process in the background
                if (kill(pid, SIGCONT) == 0) {
                    strcpy(bg_process[i]->status, "Running");
                    printf("Process %d: %s - now Running in the background\n", pid, bg_process[i]->command);
                } else {
                    perror("Error sending SIGCONT");
                }
            } else {
                printf("Process %d is not stopped.\n", pid);
            }
            return;  // Exit the function after processing
        }
    }

    if (!found) {
        printf("No such process found\n");
    }
}