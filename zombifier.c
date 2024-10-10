#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
    
void sigcont_handler( ) {
    printf("SIGCONT signal received\n");
}

int is_valid_number(const char* str, int* number) {
    // Check if empty string
    if (str[0] == '\0') {
        return 0;
    }
    
    // Check if all characters are digits
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }

    *number = atoi(str);
    return 1;
}


int main(int argc, char *argv[]) {
    int num_zombies = 0;
    
    // Parse command line arguments
    if (argc != 3 || strcmp(argv[1], "-n") != 0) {
       fprintf(stderr, "Usage: -n <number_of_zombies>\n");
    }
    
    num_zombies = atoi(argv[2]);
    if (num_zombies <= 0) {
        fprintf(stderr, "Please provide a positive number of zombies\n");
    }

    if (!is_valid_number(argv[2], &num_zombies)) {
        fprintf(stderr, "Error: '%s' is not a valid positive integer\n", argv[2]);
    }

    if(signal(SIGCONT, sigcont_handler)==SIG_ERR)
    {
        perror("signal");
        return 1;
    }
    
    printf("Creating %d zombie processes...\n", num_zombies);
    
    // Create zombie processes
    for (int i = 0; i < num_zombies; i++) {
        pid_t pid = fork();
        
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
        
        if (pid == 0) {
            // Child process
            printf("Child process %d (PID: %d) created\n", i+1, getpid());
            exit(0);  // Child exits immediately, becoming a zombie
        } else {
            // Parent process
            // Don't wait for child, letting it become a zombie
        }
    }
    
    printf("\nAll zombie processes created. Parent PID: %d\n", getpid());

    printf("parent process pausing for SIGCONT %d \n", getpid());
    printf("To verify zombies, run: ps -l | grep Z\n");
    printf("To kill zombies, run: kill -s SIGCONT <pid>\n");
    pause(); // Wait for signals

    printf("Press Enter to cleanup zombies and exit...\n");
    
    
    
    // Cleanup zombies
    for (int i = 0; i < num_zombies; i++) {
        wait(NULL);
    }
    
    printf("All zombie processes cleaned up.\n");
    
    return 0;
}