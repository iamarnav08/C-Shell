#include "specific_commands.h"
#include "headers.h"
#include "functions.h"
#include "input.h"

extern char init_home[];
extern char prev_dir[BUFFER_SIZE];
extern Process* bg_process[BUFFER_SIZE];
extern int num_background_processes;
extern Process* current_fg_process;

void hop(char* path){
    char* target_path=(char*)malloc(BUFFER_SIZE);
    char current_dir[BUFFER_SIZE];
    getcwd(current_dir, BUFFER_SIZE);

    //to go to home directory
    if(strcmp(path, "~")==0 || path[0]=='\0'){
        strcpy(target_path, init_home);
    }

    //to go to previous directory
    else if(strcmp(path, "-")==0){
        if(strlen(prev_dir)==0){
            printf("No previous directory found\n");
            return;
        }
        strcpy(target_path, prev_dir);
    }
    
    // for directory starting with ~
    else if(path[0]=='~'){
        snprintf(target_path, BUFFER_SIZE-1, "%s%s", init_home, path + 1);
    }
    
    // to stay in current directory
    else if(strcmp(path, ".")==0){
        strcpy(target_path, current_dir);
    }

    // to go to the above directory
    else if(strcmp(path, "..")==0){
        strcpy(target_path, current_dir);
        char* last_slash = strrchr(target_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';
        }
    }
    else{
        strcpy(target_path, path);
    }


    if(chdir(target_path)!=0){
        perror("Error");
        return;
    }


    // storing current directory as prev_dir for next time
    strcpy(prev_dir, current_dir);
    
    char new_dir[BUFFER_SIZE];
    if(getcwd(new_dir, BUFFER_SIZE)==NULL){
        perror("Error");
        return;
    }

    // printing the new directory
    int l1=strlen(init_home);

    if(strcmp(new_dir, init_home)==0){
        printf("~\n");
    }
    else if(strlen(new_dir)<l1){
        printf("%s\n", new_dir);
    }
    else{
        printf("~%s\n", new_dir + l1);
    }
    free(target_path);
    return;
}
