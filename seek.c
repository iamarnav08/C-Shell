#include "functions.h"


extern char prev_dir[BUFFER_SIZE];
extern int num_background_processes;

void seek(char* flags, char* what_to_find, char* directory_to_look_in){
   int search_dirs=(strchr(flags, 'd')!=NULL);
   int search_files=(strchr(flags, 'f')!=NULL);
   int execute_flag=(strchr(flags, 'e')!=NULL);

   if(search_dirs && search_files){
       printf("Invalid flags\n");
       return;
    }
    if(!search_dirs && !search_files){
        search_dirs=1;
        search_files=1;
    }

    int file_count=0, dir_count=0;
    char file_path[BUFFER_SIZE]="";
    char dir_path[BUFFER_SIZE]="";

    recursive_seek(directory_to_look_in, what_to_find, search_dirs, search_files, execute_flag, &file_count, &dir_count, file_path, dir_path);
    if(execute_flag){
        if(file_count==0 && dir_count==0){
            printf("No match found!\n");
        }
        else if(file_count==1 && dir_count==0){
            // code to print the file
            print_file(file_path);
        }
        else if(file_count==0 && dir_count==1){
            hop(dir_path);
        }
        else{
            printf("Multiple files or directories found\n");
        }
    }
    if(file_count==0 && dir_count==0){
            printf("No match found!\n");
    }
}

void recursive_seek(char* base_path, char* what_to_find, int search_dirs, int search_files, int execute_flag, int* file_count, int* dir_count, char* file_path, char* dir_path){
    if(base_path==NULL){
        base_path=".";
    }

    struct dirent* entry;
    DIR* dir=opendir(base_path);

    if(dir==NULL){
        perror("Error opening directory");
        return;
    }

    while((entry=readdir(dir))!=NULL){
        char path[BUFFER_SIZE];
        snprintf(path, BUFFER_SIZE-1, "%s/%s", base_path, entry->d_name);

        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0) continue;

        if(strncmp(entry->d_name, what_to_find, strlen(what_to_find))==0 || strcmp(what_to_find, ".")==0){
            if(entry->d_type == DT_DIR && search_dirs){
                printf("%s%s%s\n", COLOR_BLUE, entry->d_name, COLOR_RESET);
                (*dir_count)++;
                strcpy(dir_path, path);
            }
            else if(entry->d_type == DT_REG && search_files){
                printf("%s%s%s\n", COLOR_GREEN, entry->d_name, COLOR_RESET);
                (*file_count)++;
                strcpy(file_path, path);
            }
        }
        if(entry->d_type == DT_DIR){
            recursive_seek(path, what_to_find, search_dirs, search_files, execute_flag, file_count, dir_count, file_path, dir_path);
        }
    }
    closedir(dir);
}


void print_file(char* file_path){
    FILE* file=fopen(file_path, "r");
    if(file){
        char ch;
        while((ch=fgetc(file))!=EOF){
            printf("%c", ch);
        }
        fclose(file);
    }
    else{
        perror("Error opening file");
    }
    printf("\n");
}