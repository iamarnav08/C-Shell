#include "specific_commands.h"
#include "headers.h"
#include "functions.h"
#include "input.h"

extern char init_home[];
extern char prev_dir[BUFFER_SIZE];
extern Process* bg_process[BUFFER_SIZE];
extern int num_background_processes;
extern Process* current_fg_process;

void proclore(char* pid_str){
    printf("Process Information\n\n");
    printf("pir_str: %s\n", pid_str);
    pid_t pid;
    if(pid_str==NULL){
        pid=getpid();
    }
    else{
        pid=atoi(pid_str);
    }
    printf("pid: %d\n\n", pid);
    char proc_path[BUFFER_SIZE];
    snprintf(proc_path, BUFFER_SIZE-1, "/proc/%d", pid);

    int l=strlen(proc_path);

    // Get process group, although it will be printed later
    char stat_path[BUFFER_SIZE+6];
    sprintf(stat_path,"%s/stat", proc_path);
    FILE* stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        perror("Error opening stat file");
        return;
    }
    char back_or_fore[BUFFER_SIZE];
    if(fgets(back_or_fore, BUFFER_SIZE-1, stat_file)==NULL){
        perror("Error reading stat file");
        fclose(stat_file);
        return;
    }
    int tty_nr = 0;
    pid_t pgid;
    char state;
    
    fscanf(stat_file, "%*d %*s %c %*d %*d %*d %d %*d %d", &state, &tty_nr, &pgid);
    pgid=getpgid(pid);
    int shell_pid=tcgetpgrp(STDOUT_FILENO);

    char back_fore;
    // if(tcgetpgrp(STDIN_FILENO)==pgid){
    if(shell_pid==pgid){
        back_fore='+';
    }
    else{
        back_fore='\0';
    }

    

    // getting the process group for further down the line
    int process_group;
    fscanf(stat_file, "%*d %*s %*c %d", &process_group);
    fclose(stat_file);

    // Get process status
    char status_path[BUFFER_SIZE+8];
    sprintf(status_path, "%s/status", proc_path);
    FILE* status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        perror("Error opening status file");
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, BUFFER_SIZE, status_file) != NULL) {
        printf("line: %s\n", line);
        if (strncmp(line, "State:", 6) == 0) {
            printf("Process Status: %c%c\n\n", line[7], back_fore);
            break;
        }
    }
    fclose(status_file);


    // printing the process group
    printf("Process Group: %d\n\n", process_group);


    // Get virtual memory address
    char maps_path[BUFFER_SIZE+6];
    sprintf(maps_path, "%s/maps", proc_path);
    FILE* maps_file = fopen(maps_path, "r");
    if (maps_file == NULL) {
        perror("Error opening maps file");
        return;
    }

    char address[BUFFER_SIZE];
    if (fgets(line, BUFFER_SIZE, maps_file) != NULL) {
        sscanf(line, "%s", address);
        printf("Virtual Memory Address: %lld\n\n", atoll(address));
    }
    fclose(maps_file);

    // Get executable path
    char exe_path[BUFFER_SIZE+5];
    sprintf(exe_path, "%s/exe", proc_path);
    char executable_path[BUFFER_SIZE];
    ssize_t len = readlink(exe_path, executable_path, BUFFER_SIZE - 1);
    if (len != -1) {
        executable_path[len] = '\0';
        if (strncmp(executable_path, init_home, strlen(init_home)) == 0) {
            printf("Executable Path: ~%s\n\n", executable_path + strlen(init_home));
        } else {
            printf("Executable Path: %s\n\n", executable_path);
        }
    } else {
        perror("Error getting executable path");
    }
}
