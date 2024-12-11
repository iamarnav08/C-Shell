#include "functions.h"

extern char prev_dir[BUFFER_SIZE];
extern int num_background_processes;

void print_information(struct dirent* entry, char* target_path, int show_hidden, int show_long){
    struct stat file_stat;
    char full_path[BUFFER_SIZE];
    snprintf(full_path, BUFFER_SIZE-1, "%s/%s", target_path, entry->d_name);
    stat(full_path, &file_stat);

    // printf("%s\n", entry->d_name);

    if(show_hidden==0 && entry->d_name[0]=='.'){
        return;
    }

    if(show_long){
        char mode[11] = "----------";
        if (S_ISDIR(file_stat.st_mode)) mode[0] = 'd';
        if (file_stat.st_mode & S_IRUSR) mode[1] = 'r';
        if (file_stat.st_mode & S_IWUSR) mode[2] = 'w';
        if (file_stat.st_mode & S_IXUSR) mode[3] = 'x';
        if (file_stat.st_mode & S_IRGRP) mode[4] = 'r';
        if (file_stat.st_mode & S_IWGRP) mode[5] = 'w';
        if (file_stat.st_mode & S_IXGRP) mode[6] = 'x';
        if (file_stat.st_mode & S_IROTH) mode[7] = 'r';
        if (file_stat.st_mode & S_IWOTH) mode[8] = 'w';
        if (file_stat.st_mode & S_IXOTH) mode[9] = 'x';

        char time_str[20];
        struct tm *timeinfo;
        timeinfo = localtime(&file_stat.st_mtime);
        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);

        printf("%s %ld %s %s %ld %s ",
               mode,
               file_stat.st_nlink,
               getpwuid(file_stat.st_uid)->pw_name,
               getgrgid(file_stat.st_gid)->gr_name,
               file_stat.st_size,
               time_str);
    }   
    if (S_ISDIR(file_stat.st_mode)){
        printf("\033[1;34m%s\033[0m\n", entry->d_name);  // Blue for directories
        } 
        else if(file_stat.st_mode & S_IXUSR){
            printf("\033[1;32m%s\033[0m\n", entry->d_name);  // Green for executables
        } 
        else{
            printf("\033[1;37m%s\033[0m\n", entry->d_name);  // White for files
        }

}
