#include "functions.h"

extern int num_background_processes;

void load_myshrc(User_Function* user_functions, int* user_function_count){
    int index=*user_function_count;
    FILE* file=fopen(".myshrc", "r");
    if(file==NULL){
        perror("Failed to open .myshrc");
        return;
    }
    char line[BUFFER_SIZE];
    char command_buffer[BUFFER_SIZE]="";
    int in_function=0;
    while(fgets(line, BUFFER_SIZE, file)){
        // Assuming comments are always in their separate lines
        // We will skip lines beginning with #
        if(line[0]=='#'){
            continue;
        }
        if(strlen(line)==0){
            continue;
        }

        if(strstr(line, "()") && strstr(line, "{")){
            char* saveptr;  // `strtok_r` uses this for context
            char temp_line[BUFFER_SIZE];
            strcpy(temp_line, line);  // Make a copy to use with strtok_r

            char* alias11 = strtok_r(temp_line, "(", &saveptr);
            if (alias11) {
                // printf("alias at here: %s\n", alias11);
                in_function = 1;
                command_buffer[0] = '\0';  // Clear the command buffer

                while (in_function && fgets(line, BUFFER_SIZE, file)) {
                    line[strcspn(line, "\n")] = 0;  // Remove newline character
                    if (strstr(line, "}")) {
                        in_function = 0;
                        break;
                    }
                    strcat(command_buffer, line);
                    strcat(command_buffer, "; ");
                }

                printf("command_buffer: %s\n", command_buffer);
                strcpy(user_functions[index].alias, alias11);
                strcpy(user_functions[index].actual_command, command_buffer);
                command_buffer[0] = '\0';  // Reset buffer for the next function
                index++;
            }
        }
        else if(strchr(line, '=')){
            char alias[BUFFER_SIZE], actual_command[BUFFER_SIZE];
            sscanf(line, "alias %[^=]=%[^\n]", alias, actual_command);
            strncpy(user_functions[index].alias, alias, strlen(alias)-1);
            strcpy(user_functions[index].actual_command, actual_command);
            index++;
        }
    }
    *user_function_count=index;
    
    fclose(file);
}

