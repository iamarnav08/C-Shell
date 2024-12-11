#ifndef INPUT_H
#define INPUT_H

#include "functions.h"
#include "log.h"

void Take_input(char* Input);

void tokenise_piped_commands(char* piped_command, char* args1, char** input_file, char** output_file, int* append, int* i);

void check_redirected_input(char* input_file);

void check_redirected_output(char* output_file, int* append);

void process_command(char* command, int* store_check, log* LOG, int* run, User_Function* user_functions, int user_function_count);

void execute_command(char* command, int background, log* LOG, int* run);

#endif

