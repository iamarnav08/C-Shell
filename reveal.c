#include "functions.h"

extern char init_home[];
extern char prev_dir[BUFFER_SIZE];
extern int num_background_processes;

void reveal(char* flags, char* path, int l, int a){
    int show_hidden=0;
    int show_long=0;
    if(l){
        show_long=1;
    }
    if(a){
        show_hidden=1;
    }


    char target_path[BUFFER_SIZE];
    if(path==NULL){
        getcwd(target_path, BUFFER_SIZE);
    }
    else{
        if(strcmp(path, "~")==0){
            strcpy(target_path, init_home);
        }
        else if(path[0]=='~'){
            snprintf(target_path, BUFFER_SIZE-1, "%s%s", init_home, path + 1);
        }
        else if(strcmp(path, ".")==0){
            getcwd(target_path, BUFFER_SIZE);
        }
        else if(strcmp(path, "..")==0){
            getcwd(target_path, BUFFER_SIZE);
            char* last_slash = strrchr(target_path, '/');
            if (last_slash != NULL) {
                *last_slash = '\0';
            }
        }
        else if(strcmp(path, "-")==0 && strlen(path)==1){
            if(strlen(prev_dir)==0){
                printf("No previous directory found\n");
                return;
            }
        }
        else{
            strcpy(target_path, path);
        }
    }

    DIR* dir=opendir(target_path);
    if(dir==NULL){
        perror("Error");
        return;
    }

    struct dirent* entry;
    while((entry=readdir(dir))!=NULL){
        print_information(entry, target_path, show_hidden, show_long);
    }
    closedir(dir);

    getcwd(target_path, BUFFER_SIZE);
}
