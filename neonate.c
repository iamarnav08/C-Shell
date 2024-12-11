#include "functions.h"

extern int num_background_processes;



void enable_raw_mode(struct termios *orig_termios) {
    struct termios raw;
    
    // Get the original terminal settings
    tcgetattr(STDIN_FILENO, orig_termios);
    raw = *orig_termios;

    // Modify the terminal settings to raw mode
    raw.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios *orig_termios) {
    // Restore the original terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

#define X_KEY 'x'

int is_x_pressed() {
    // Save the original terminal settings
    static struct termios orig_termios, raw;
    static int initialized = 0;

        // Get the current terminal settings
        tcgetattr(STDIN_FILENO, &orig_termios);

        // Set terminal to raw mode
        raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);

        // Set the file descriptor to non-blocking
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

     

    // Check for available input
    char ch;
    ch=getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    fcntl(STDIN_FILENO, F_SETFL, raw);

    if (ch != EOF) {
        if (ch == 'x') {
            return 1;
        }
    }
    return 0;
}

void neonate(int time_arg) {
    struct termios orig_termios;
    enable_raw_mode(&orig_termios); // Enable raw mode

    int run=1;
    while(run){
        FILE* f = fopen("/proc/sys/kernel/ns_last_pid","r"); 
        char pid[10];
        fgets(pid,10,f);
        printf("most_recent_pid:%s",pid);
        for (int i = 0; i < time_arg * 10; ++i) {  // Loop to sleep in 0.1 second increments
            usleep(100000);  // Sleep for 0.1 second
            if (is_x_pressed()) {
                run = 0;
                break;
            }
        }
        if(!run){
            break;
        }
    }

    disable_raw_mode(&orig_termios); // Restore terminal settings
    printf("Terminating...\n");
}
