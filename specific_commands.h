#ifndef SPECIFIC_COMMANDS_H
#define SPECIFIC_COMMANDS_H

#include "headers.h"


#define COLOR_GREEN "\033[0;32m"  // Files
#define COLOR_BLUE "\033[0;34m"   // Directories
#define COLOR_RESET "\033[0m"



void hop(char* path);

void reveal(char* flags, char* path, int l, int a);

void print_information(struct dirent* entry, char* target_path, int show_hidden, int show_long);

void proclore(char* pid_str);

void search_directory(const char* base_path, const char* target, int only_dirs, int only_files, int execute_flag, int* found_file, int* found_dir);
    
void seek(char* flags, char* what_to_find, char* directory_to_look_in);
    
void recursive_seek(char* base_path, char* what_to_find, int search_dirs, int search_files, int execute_flag, int* file_count, int* dir_count, char* file_path, char* dir_path);

void print_file(char* file_path);

void remove_html_tags(const char *input);

void fetch_man_page(const char *command);

void fg(int pid);

#endif
