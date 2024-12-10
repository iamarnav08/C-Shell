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

prompt_strings* get_prompt_strings(){
    prompt_strings* A = (prompt_strings*)malloc(sizeof(prompt_strings));
    if (A == NULL) {
        perror("Failed to allocate memory for prompt_strings");
        return NULL;
    }

    A->user_name = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    if (A->user_name == NULL) {
        perror("Failed to allocate memory for user_name");
        free(A);
        return NULL;
    }

    A->directory = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    if (A->directory == NULL) {
        perror("Failed to allocate memory for directory");
        free(A);
        return NULL;
    }

    A->host_name = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    if (A->host_name == NULL) {
        perror("Failed to allocate memory for host_name");
        free(A->directory);
        free(A);
        return NULL;
    }

    if(getlogin_r(A->user_name, BUFFER_SIZE) != 0){
        perror("Failed to get user name");
        free(A->user_name);
        free(A->directory);
        free(A->host_name);
        free(A);
        return NULL;
    }

    if (gethostname(A->host_name, BUFFER_SIZE) != 0) {
        perror("Failed to get host name");
        free(A->host_name);
        free(A->directory);
        free(A);
        return NULL;
    }

    if (getcwd(A->directory, BUFFER_SIZE) == NULL) {
        perror("Failed to get current working directory");
        free(A->host_name);
        free(A->directory);
        free(A);
        return NULL;
    }

    return A;
}

void print_prompt(int* home_dir_len, char* home_dir){
    prompt_strings* A=get_prompt_strings();
    printf("\033[0m");
    printf("<");
    printf("\033[38;5;51m");
    printf("%s", A->user_name);
    printf("\033[0m");
    // printf("@%s>", A->directory);
    int l1=*home_dir_len;
    int l2=strlen(A->directory);
    if(strcmp(A->directory, home_dir)==0){
        strcpy(A->directory, "~");
        printf("@%s:%s",A->host_name, A->directory);
    }
    else if(strlen(A->directory)<l1){
        printf("@%s:%s",A->host_name, A->directory);
    }
    else{
        char* new_dir=(char*)malloc(l2-l1);
        strncpy(new_dir, A->directory+l1+1, l2-l1-1);
        new_dir[l2-l1-1]='\0';
        A->directory=new_dir;
        printf("@%s:/%s",A->host_name, A->directory);
    }

    // to display other info if process takes to long
    if(last_fg_time>0){
        printf(" %s : %ds", last_fg_command, last_fg_time);
    }
    printf(">");   

    free(A->directory);
    free(A->host_name);
    free(A);

}


