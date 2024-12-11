#include "log.h"

extern char init_home[BUFFER_SIZE];
extern char prev_dir[BUFFER_SIZE];

log* init_log(){
    log* LOG=(log*)malloc(sizeof(log));
    if(LOG==NULL){
        perror("Failed to allocate memory for log");
        return NULL;
    }
    LOG->size=0;
    LOG->log_index=0;
    LOG->count=0;
    return LOG;
}

void load_log(log* LOG){
    FILE *file=fopen(LOG_FILE, "r");
    if(file==NULL){
        file=fopen(LOG_FILE, "w");
        if(file==NULL){
            perror("Failed to open log file");
            return;
        }
    }
    else{
        char line[BUFFER_SIZE];
        while(fgets(line, BUFFER_SIZE, file)){
            line[strcspn(line, "\n")] = 0;
            strcpy(LOG->command_log[LOG->size++], line);
            if(LOG->size >= LOG_FILE_SIZE){
                LOG->log_index = (LOG->log_index + 1) % LOG_FILE_SIZE;
                LOG->size--;
            }
        }
    }
    fclose(file);
}

void save_log(log* LOG){
    FILE* file=fopen(LOG_FILE, "w");
    if(file==NULL){
        perror("Failed to open log file");
        return;
    }
    // printf("saving log\n");
    int idx=LOG->log_index;
    for(int i=0; i<LOG->size; i++){
        fprintf(file, "%s\n", LOG->command_log[idx]);
        idx=(idx+1)%LOG_FILE_SIZE;
    }
    fclose(file);
}

void write_log(char* command, log* LOG){
    if(LOG->size>0 && strcmp(command, LOG->command_log[(LOG->log_index+LOG->size-1)%LOG_FILE_SIZE])==0){
        return;     // do not write the same command twice
    }
    strcpy(LOG->command_log[(LOG->log_index+LOG->size)%LOG_FILE_SIZE], command);
    if(LOG->size<LOG_FILE_SIZE){
        LOG->size++;
    }
    else{
        LOG->log_index=(LOG->log_index+1)%LOG_FILE_SIZE;
    }
}

void print_log(log* LOG){
    int idx=LOG->log_index;
    for(int i=0; i<LOG->size; i++){
        printf("%s\n", LOG->command_log[idx]);
        idx=(idx+1)%LOG_FILE_SIZE;
    }
}

void purge_log(log* LOG){
    LOG->size=0;
    LOG->log_index=0;
    for(int i=0; i<LOG_FILE_SIZE; i++){
        memset(LOG->command_log[i], 0, BUFFER_SIZE);
    }
    // LOG_FILE should be cleared after the log purge command
    FILE* file=fopen(LOG_FILE, "w");
    if(file==NULL){
        perror("Failed to open log file");
        return;
    }
    fclose(file);
}

void execute_log(log* LOG, int index){
    int idx;
    idx=(LOG->size + LOG->log_index - index)%LOG_FILE_SIZE;
    char* command_1=(char*)malloc(BUFFER_SIZE);
    if(command_1==NULL){
        perror("Failed to allocate memory for command");
        return;
    }
    strcpy(command_1, LOG->command_log[idx]);
    char* rem_log_command;
    char* command=strtok_r(command_1, ";", &rem_log_command);
    while(command!=NULL){
            process_execute(command);
            command=strtok_r(NULL, ";", &rem_log_command);
            
        }
}

void process_execute(char* command){
    while(isspace((unsigned char)command[strlen(command)-1])){
        command[strlen(command)-1]='\0';
    }
    char temp_command[BUFFER_SIZE]="";
    int background=0;
    char* rem_command;
        char* and_token=strtok_r(command, " \t", &rem_command);
        while(and_token!=NULL){
        if(strcmp(and_token, "&")==0){
            background=1;
            execute_command_log(temp_command, background);
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
        execute_command_log(temp_command, background);
    }
    
    command[0]='\0';

}

void execute_command_log(char* command, int background){ 
    char* args1[BUFFER_SIZE];
    int i = 0;

    // Tokenize the command into arguments
    char* token = strtok(command, " \t");
    while (token != NULL) {
        args1[i++] = token;
        token = strtok(NULL, " \t");
    }
    args1[i] = NULL; // Null-terminate the argument list
    if(strcmp(args1[0],"hop")==0){
        if(i>1){
            for(int j=1; j<i; j++){
                hop(args1[j]);
            }
        }
        else{
            hop("~");
            
        }
        return;
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
        return;
    }
    else if(strcmp(args1[0], "proclore")==0){
                proclore(args1[1]);
                return;
            }

   

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args1[0], args1) == -1) {
            perror("execvp failed");
            _exit(1); // Exit if execvp fails
        }
        
    } else if (pid > 0) {
        // Parent process
        if (background) {
            printf("Process ID: %d\n", pid);
        } else {
            waitpid(pid, NULL, 0); // Wait for the child process to finish
        }
    } else {
        perror("fork failed");
    }
}

void log_hop(char* path){
    
    char* target_path=(char*)malloc(BUFFER_SIZE);
    char current_dir[BUFFER_SIZE];
    getcwd(current_dir, BUFFER_SIZE);

    //to go to home directory
    if(strcmp(path, "~")==0){
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
        snprintf(target_path, BUFFER_SIZE, "%s%s", init_home, path + 1);
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

   
    free(target_path);
}
