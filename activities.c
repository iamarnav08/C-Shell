#include "headers.h"
#include "functions.h"
#include "input.h"
#include "specific_commands.h"
#include "log.h"
#include <signal.h>
#include <unistd.h>

extern char init_home[];
extern int pid_shell;
extern int last_fg_time;
extern char last_fg_command[BUFFER_SIZE];
extern Process* process_list_head;
extern Process* bg_process[BUFFER_SIZE];
extern Process* current_fg_process;
extern int num_background_processes;

void list_activities() {
    // printf("count: %d\n", num_background_processes);
    Process* sorted[max_num_background_processes];
    int proc_num[max_num_background_processes];
    int count=0;

    for (int i = 0; i < max_num_background_processes; i++)
    {
        sorted[i] = (Process*)malloc(sizeof(Process));
        if (bg_process[i]!=NULL && bg_process[i]->pid != -1)
        {
            strcpy(sorted[count]->command, bg_process[i]->command);
            sorted[count]->pid = bg_process[i]->pid;
            strcpy(sorted[count]->status, bg_process[i]->status);
            proc_num[count] = i + 1;
            count++;
        }
    }
    // printf("count: %d\n", count);
    // printf("num_background_processes: %d\n", num_background_processes);
    //sorting the processes
    for (int i = 0; i < count; i++){
        for (int j = i + 1; j < count; j++){
            if (strcmp(sorted[i]->command, sorted[j]->command) > 0){
                // Process* temp = sorted[i];
                // sorted[i] = sorted[j];
                // sorted[j] = temp;
                // int temp1 = proc_num[i];
                // proc_num[i] = proc_num[j];
                // proc_num[j] = temp1;
                char name_temp[BUFFER_SIZE];
                char status_temp[BUFFER_SIZE];
                int pid_temp;

                strcpy(name_temp, sorted[i]->command);
                strcpy(sorted[i]->command, sorted[j]->command);
                strcpy(sorted[j]->command, name_temp);

                strcpy(status_temp, sorted[i]->status);
                strcpy(sorted[i]->status, sorted[j]->status);
                strcpy(sorted[j]->status, status_temp);

                pid_temp = sorted[i]->pid;
                sorted[i]->pid = sorted[j]->pid;
                sorted[j]->pid = pid_temp;
            }
        }
    }
    // for(int i=0; i<count; i++){
    //     printf("command: %s, pid: %d, status: %s\n", sorted[i]->command, sorted[i]->pid, sorted[i]->status);
    // }
    FILE *file;
    // now we go through the sorted array and find their states, whether they are running or stopped
    
    // then we print the processes
    for (int i = 0; i < count; i++) {
        pid_t pid = (int)sorted[i]->pid;
        // printf("pid: %d\n", pid);
        char path[BUFFER_SIZE];
        char buffer[BUFFER_SIZE];
        char* proc_details[BUFFER_SIZE]={NULL};
        int a=0;
        

        // Construct the path to the /proc/[pid]/stat file
        sprintf(path, "/proc/%d/stat", pid);

        // Open the file
        file = fopen(path, "r");
        if (file == NULL) {
            printf("Error opening file\n");
            return;
        }
        fread(buffer, BUFFER_SIZE, 1, file);
        // printf("buffer: %s\n", buffer);
        fclose(file);
        // printf("ouhoqh123\n");
        // Split the contents of the file by spaces
        char* token=strtok(buffer, " ");
        proc_details[i]=token;
        while(token != NULL){
            a++;
            token=strtok(NULL, " ");
            proc_details[a]=token;
        }
        int stat=kill(pid, SIGSTOP);
        kill(pid, SIGCONT);

        if(stat==0){
            printf("Running\n");
            strcpy(sorted[i]->status, "Running");
        }
        else{
            printf("Stopped\n");
            strcpy(sorted[i]->status, "Stopped");
        }

        printf("%d: %s - %s\n", pid, sorted[i]->command, sorted[i]->status);

    }   
    for (int i = 0; i < max_num_background_processes; i++)
    {
        free(sorted[i]);
    }
}

