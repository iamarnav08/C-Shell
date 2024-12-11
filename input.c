#include "input.h"

int num_background_processes = 0;
int foreground_pid;

extern Process* bg_process[];
extern Process* current_fg_process;

void Take_input(char* Input){
    if(fgets(Input, BUFFER_SIZE, stdin)==NULL){
        kill(0, SIGKILL);
        exit(0);
    }
    size_t len = strlen(Input);
    if (len > 0 && Input[len - 1] == '\n') {
        Input[len - 1] = '\0';
    }
}

void trim_leading_whitespace(char* str) {
    int index = 0;
    
    // Find the index of the first non-whitespace character
    while (str[index] != '\0' && isspace((unsigned char)str[index])) {
        index++;
    }

    // Shift the string left by 'index' positions
    if (index > 0) {
        int i = 0;
        while (str[index] != '\0') {
            str[i++] = str[index++];
        }
        str[i] = '\0';  // Null-terminate the trimmed string
    }
}


void process_command(char* command, int* store_check, log* LOG, int* run, User_Function* user_functions, int user_function_count){
    while(isspace((unsigned char)command[strlen(command)-1])){
        command[strlen(command)-1]='\0';
    }
    for(int i=0; i<user_function_count; i++){
                char* command_copy=(char*)malloc(BUFFER_SIZE);
                strcpy(command_copy, command);
                char* check;
                check=strtok(command_copy, " ");
                if(strcmp(check, user_functions[i].alias)==0){
                    strcpy(command, user_functions[i].actual_command);
                    i=user_function_count+1;
                    break;
                }
                
            }
    char temp_command[BUFFER_SIZE]="";
    int background=0;
    char* rem_command;
    char* and_token=strtok_r(command, " \t", &rem_command);
    while(and_token!=NULL){
        if(strcmp(and_token, "log")==0){
            *store_check=0;
        }
        if(strcmp(and_token, "&")==0){
            background=1;
            execute_command(temp_command, background, LOG, run);
            temp_command[0]='\0';
            background=0;
        }
        else{
            if(strlen(temp_command)!=0){
                strcat(temp_command, " ");
            }
            strcat(temp_command, and_token);
        }
        and_token=strtok_r(NULL, " \t", &rem_command);
    }
    if(temp_command[0]!='\0'){
        execute_command(temp_command, background, LOG, run);
    }
    command[0]='\0';
}


int last_fg_time=0;
char last_fg_command[BUFFER_SIZE]="";

void execute_command(char* command, int background, log* LOG, int* run){
    char* piped_tokens[BUFFER_SIZE];
    int num_pipes=0;
    
    int i = 0; 

    char* input_file = NULL;
    char* output_file = NULL;
    int append=0;

    //saving original file descriptors
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);

    //Tokenising for pipes
    char* token_pip_rem;
    char* token_pipe = strtok_r(command, "|", &token_pip_rem);
    while(token_pipe!=NULL){
        piped_tokens[num_pipes++]=token_pipe;
        token_pipe=strtok_r(NULL, "|", &token_pip_rem);
    }

    // handling case of empty before or after pipe symbol
    for(int j=0; j<num_pipes; j++){
        trim_leading_whitespace(piped_tokens[j]);
        if(strlen(piped_tokens[j])==0){
            printf("Invalid command\n");
            return;
        }
    }

    // handling input and output redirection for pipes commands
    int pipefd[2*(num_pipes-1)];
    for(int j=0; j<num_pipes; j++){
        if(pipe(pipefd + j*2) < 0) {
            perror("Pipe failed");
            exit(1);
        }
    }
    pid_t pid;
    struct timeval start, end;
    int status;
    for(int j=0; j<num_pipes; j++){
        char* args1[BUFFER_SIZE];
        i=0;
        // Tokenize the command into arguments
        char* interior_token_rem;
        char* token = strtok_r(piped_tokens[j], " \t", &interior_token_rem);
        // tokenising for input and output redirection
        while (token != NULL) {
        if(strcmp(token, "<")==0){
            token = strtok_r(NULL, " \t", &interior_token_rem);
            if(token==NULL){
                printf("No input file specified\n");
                return;
            }
            input_file=token;
        }
        else if(strcmp(token, ">")==0){
            token = strtok_r(NULL, " \t", &interior_token_rem);
            if(token==NULL){
                printf("No output file specified\n");
                return;
            }
            output_file=token;
            append=0;
        }
        else if(strcmp(token, ">>")==0){
            token = strtok_r(NULL, " \t", &interior_token_rem);
            if(token==NULL){
                printf("No output file specified\n");
                return;
            }
            output_file=token;
            append=1;
        }
        else{
            args1[i++] = token;
        }
        token = strtok_r(NULL, " \t", &interior_token_rem);
    }
    args1[i] = NULL; // Null-terminate the argument list
        

    if(background==0){ //foreground process
        gettimeofday(&start, NULL); // start time
        pid=fork();

        if(pid<0){
            perror("fork failed");
        }
        else if(pid==0){
            // handling input redirection
            if(input_file!=NULL){
                int fd_in = open(input_file, O_RDONLY | O_CREAT, 0644);
                if(fd_in<0){
                    perror("Input file not found");
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // handling output redirection
            if(output_file!=NULL){
                int fd_out;
                if(append){
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                }
                else{
                    fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                if(fd_out<0){
                    perror("Output file not found");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            // handling pipes
            if(j>0){
                dup2(pipefd[(j-1)*2], STDIN_FILENO);
            }
            if(j<num_pipes-1){
                dup2(pipefd[j*2+1], STDOUT_FILENO);
            }
            for(int k=0; k<2*(num_pipes-1); k++){
                close(pipefd[k]);
            }

            if(strcmp(args1[0],"hop")==0){
                if(input_file==NULL){
                    if(i>1){
                    for(int j=1; j<i; j++){
                        hop(args1[j]);
                    }
                    }
                    else{
                        hop("~");
                        
                    }
                    dup2(original_stdin, STDIN_FILENO);
                    close(original_stdin);
                    dup2(original_stdout, STDOUT_FILENO);
                    close(original_stdout);
                }
                else{
                    char* path = (char*)malloc(BUFFER_SIZE);  // Allocate memory for the path
                    if (path == NULL) {
                        perror("Failed to allocate memory");
                        exit(1);
                    }
                    while(read(STDIN_FILENO, path, BUFFER_SIZE)!=0){
                        // Remove any trailing newline character that fgets might add
                        size_t len = strlen(path);
                        if (len > 0 && path[len - 1] == '\n') {
                            path[len - 1] = '\0';
                        }
                        if (path != NULL) {
                        // If the path is empty, print a message and return
                        if (strlen(path) == 0) {
                            printf("No path specified\n");
                        } else {
                            hop(path);  // Call the hop function with the path read from the file
                        }
                    } 
                        if (feof(stdin)) {
                            perror("End of input file reached\n");
                        } else if (ferror(stdin)) {
                            perror("Error reading input file");
                        }
                        break;
                    
                    }
                    // Restore original stdin and stdout
                    dup2(original_stdin, STDIN_FILENO);
                    close(original_stdin);
                    dup2(original_stdout, STDOUT_FILENO);
                    close(original_stdout);
                    free(path);  // Free the allocated memory
                }
            }
            else if(strcmp(args1[0], "reveal")==0){
                char* flags=NULL;
                char* path=NULL;
                int l=0; 
                int a=0;

                for(int j=1; j<i; j++){
                    if(args1[j][0]=='-' && strlen(args1[j])>1){
                        flags=args1[j];
                        if(strchr(flags, 'l')!=NULL){
                            l=1;
                        }
                        if(strchr(flags, 'a')!=NULL){
                            a=1;
                        }
                    }
                    else{
                        path=args1[j];
                    }
                }
                reveal(flags, path, l, a);
                exit(0);
            }
            else if(strcmp(args1[0], "seek")==0){
                char flags[BUFFER_SIZE]="";
                char* what_to_find=NULL;
                char* directory_to_look_in=NULL;
                int flag_index=1;

                if(input_file==NULL){
                    for(int j=1; j<i; j++){
                    if(args1[j][0]=='-'){
                        strcat(flags, args1[j]);
                        flag_index++;
                    }
                    else if(what_to_find==NULL){
                        what_to_find=args1[j];
                        flag_index++;
                    }
                    else if(directory_to_look_in==NULL){
                        directory_to_look_in=args1[j];
                        flag_index++;
                    }
                    }
                    if(what_to_find==NULL){
                        printf("No target specified\n");
                        return;
                    }
                    seek(flags, what_to_find, directory_to_look_in);
                }
                else{
                    char* line = (char*)malloc(BUFFER_SIZE);  // Allocate memory for the path
                    if (line == NULL) {
                        perror("Failed to allocate memory");
                        exit(1);
                    }

                    // Read one line from stdin (which is redirected from the file)
                    if (fgets(line, BUFFER_SIZE, stdin) != NULL) {
                        // Remove any trailing newline character that fgets might add
                        size_t len = strlen(line);
                        if (len > 0 && line[len - 1] == '\n') {
                            line[len - 1] = '\0';
                        }

                        // If the path is empty, print a message and return
                        if (strlen(line) == 0) {
                        } else {
                            char* seek_token_rem;
                            char* seek_token = strtok_r(line, " \t", &seek_token_rem);
                            while(seek_token!=NULL){
                                if(seek_token[0]=='-'){
                                    strcat(flags, seek_token);
                                    flag_index++;
                                }
                                else if(what_to_find==NULL){
                                    what_to_find=seek_token;
                                    flag_index++;
                                }
                                else if(directory_to_look_in==NULL){
                                    directory_to_look_in=seek_token;
                                    flag_index++;
                                }
                                else{
                                    printf("Invalid input\n");
                                    break;
                                }
                                seek_token = strtok_r(NULL, " \t", &seek_token_rem);
                            }
                            if(what_to_find==NULL){
                                printf("No target specified\n");
                                exit(1);
                            }
                            seek(flags, what_to_find, directory_to_look_in);
                        }
                    } else if (feof(stdin)) {
                        printf("End of input file reached\n");
                    } else {
                        printf("Failed to read path from input file\n");
                    }
                    // Restore original stdin and stdout
                    free(line);  // Free the allocated memory
                    dup2(original_stdin, STDIN_FILENO);
                    close(original_stdin);
                    dup2(original_stdout, STDOUT_FILENO);
                    close(original_stdout);
                    exit(0);
                }
                exit(0);
            }
            else if(strcmp(args1[0], "log")==0){
                if(input_file==NULL){
                    // log will print the log file
                    if(args1[1]==NULL){
                        //code to print the log file
                        print_log(LOG);
                    }
                    else if(strcmp(args1[1], "purge")==0){
                        // code to clear the log file
                        purge_log(LOG);
                    }
                    else if(strcmp(args1[1], "execute")==0 && args1[2]!=NULL){
                        // code to execute the command in the log file
                        execute_log(LOG, atoi(args1[2]));
                    }
                }
                else{
                    printf("Reading from input redirection...\n");
                    char* line=(char*)malloc(BUFFER_SIZE);
                    if(line==NULL){
                        perror("Failed to allocate memory");
                        exit(1);
                    }
                    if(read(STDIN_FILENO, line, BUFFER_SIZE)>0){
                        printf("line: %s\n", line);

                    }
                    char* order, *rem, *order_number;
                    order=strtok_r(line, " \t", &rem);
                    order_number=strtok_r(NULL, " \t", &rem);
                    if(order==NULL){
                        //code to print the log file
                        print_log(LOG);
                    }
                    else if(strcmp(order, "purge")==0){
                        // code to clear the log file
                        purge_log(LOG);
                    }
                    else if(strcmp(order, "execute")==0 && order_number!=NULL){
                        // code to execute the command in the log file
                        execute_log(LOG, atoi(order_number));
                    }
                    free(line);
                    free(order);
                    free(order_number);
                    free(rem);
                }
                
                // Restore original stdin and stdout
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);

                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
            }
            else if(strcmp(args1[0], "quit")==0){
                printf("run: %d\n", *run);
                *run=0;
                printf("run: %d\n", *run);
                return;
            }
            else if(strcmp(args1[0], "proclore")==0){
                if(input_file==NULL){
                    if(args1[1]==NULL){
                    printf("No PID specified\n");
                    }
                    proclore(args1[1]);
                }
                else{
                    char* line=(char*)malloc(BUFFER_SIZE);
                    if(line==NULL){
                        perror("Failed to allocate memory");
                        exit(1);
                    }
                    if(read(STDIN_FILENO, line, BUFFER_SIZE)>0){
                        proclore(line);
                    }
                    free(line);
                    dup2(original_stdin, STDIN_FILENO);
                    close(original_stdin);

                    dup2(original_stdout, STDOUT_FILENO);
                    close(original_stdout);
                }
            }
            else if(strcmp(args1[0], "iMan")==0){
                if(input_file==NULL){
                    if(args1[1]==NULL){
                        printf("No page specified\n");
                    }
                    else{
                        fetch_man_page(args1[1]);
                    }
                }
                else{
                    char* line=(char*)malloc(BUFFER_SIZE);
                    if(line==NULL){
                        perror("Failed to allocate memory");
                        exit(1);
                    }
                    if(read(STDIN_FILENO, line, BUFFER_SIZE)>0){
                        char* iman_token=strtok(line, " \t");
                        fetch_man_page(iman_token);
                    }
                    free(line);
                    dup2(original_stdin, STDIN_FILENO);
                    close(original_stdin);
                    dup2(original_stdout, STDOUT_FILENO);
                    close(original_stdout);
                }
            }
            else if(strcmp(args1[0], "activities")==0){
                list_activities();
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
            }
            else if(strcmp(args1[0], "ping")==0){
                ping_process(atoi(args1[1]), atoi(args1[2]));
                dup2(original_stdin, STDIN_FILENO);
                close(original_stdin);
                dup2(original_stdout, STDOUT_FILENO);
                close(original_stdout);
            }
            else if(strcmp(args1[0], "fg")==0){
                fg(atoi(args1[1]));
            }
            else if(strcmp(args1[0], "bg")==0){
                bg(atoi(args1[1]));
            }
            else if(strcmp(args1[0], "neonate")==0){
                neonate(atoi(args1[2]));
            }
            else{
                if (execvp(args1[0], args1) < 0) {
                printf("'%s' is not a valid command\n", args1[0]);
                exit(1);
            }
            }
        }
        else{
            current_fg_process->pid=pid;
            strcpy(current_fg_process->command, command);
            if(j>0){
                close(pipefd[(j-1)*2]);
            }
            if(j<num_pipes-1){
                close(pipefd[j*2+1]);
            }
            waitpid(pid, &status, WUNTRACED);
            
            gettimeofday(&end, NULL); // end time

            current_fg_process->pid=-1;

            int elapsed = (end.tv_sec - start.tv_sec);
            if(elapsed>2){
                last_fg_time=elapsed;
                strcpy(last_fg_command, args1[0]);
            }
            else{
                last_fg_time=0;
                last_fg_command[0]='\0';
            }
        }
    }
    else{// background process
        if(num_background_processes>=max_num_background_processes){
            printf("Error- Too many background processes\n");
            return;
        }
        pid=fork();
        if(pid<0){
            perror("fork failed");
        }
        else if(pid==0){
            if (execvp(args1[0], args1) < 0) {
                perror("Exec failed");
                exit(1);
            }
        }
        else{
            add_process(pid, command, "Running");
            printf("Process ID: %d\n", pid);
            for(int k=0; k<2*num_pipes; k++){
                close(pipefd[k]);
            }
        }
    }
    }

    dup2(original_stdin, STDIN_FILENO);
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdin);
    close(original_stdout);
}
