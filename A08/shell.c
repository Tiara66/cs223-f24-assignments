#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <pwd.h>
#include <errno.h>
#include <signal.h>

#define MAX_LINE 1024 // Maximum length of input line

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
 * MyShell Program
 *
 * This shell program allows users to execute commands interactively.
 * It provides a colorful prompt with user, host, and current directory
 * information, and includes a history feature. The shell handles common
 * signals and displays the exit status or signal information when a
 * command completes.
 *
 * @author: Tianyun Song
 * @version: October 31, 2024
 */

/**
 * Displays the initial header with colorful lines and "Rainbow shell" message.
 */
void display_header() {
    printf(ANSI_COLOR_CYAN "<3 Rainbow shell <3" ANSI_COLOR_RESET "\n"
           ANSI_COLOR_RED "--------" ANSI_COLOR_GREEN 
           "--------" ANSI_COLOR_YELLOW "--------" 
           ANSI_COLOR_BLUE "--------\n" ANSI_COLOR_RESET
           ANSI_COLOR_MAGENTA "--------" ANSI_COLOR_CYAN 
           "--------" ANSI_COLOR_RED "--------" ANSI_COLOR_GREEN
            "--------\n" ANSI_COLOR_RESET);
}

/**
 * Displays a custom prompt with colorful user, host, and cwd information.
 *
 * @param prompt Buffer to store formatted prompt.
 */
void display_prompt(char *prompt) {
    char cwd[MAX_LINE];
    struct passwd *pw = getpwuid(geteuid());  // Get username
    char hostname[MAX_LINE];

    // Get current working directory and hostname
    if (getcwd(cwd, sizeof(cwd)) != NULL && gethostname(hostname, 
        sizeof(hostname)) == 0) {
        // Format the prompt
        snprintf(prompt, MAX_LINE,
                 ANSI_COLOR_RED "%.10s" ANSI_COLOR_RESET
                 "@" ANSI_COLOR_GREEN "%.10s" ANSI_COLOR_RESET
                 ":" ANSI_COLOR_YELLOW "%.50s" ANSI_COLOR_RESET
                 " " ANSI_COLOR_BLUE "<3 " ANSI_COLOR_MAGENTA 
                 "-----" ANSI_COLOR_CYAN " -----" ANSI_COLOR_RESET " ",
                 pw->pw_name, hostname, cwd);
    } else {
        perror("Error getting prompt information");
        snprintf(prompt, MAX_LINE, "MyShell> ");
    }
}

int main() {
    char *line;  // Pointer to store user input
    char *args[MAX_LINE / 2 + 1];  // Array to store command arguments
    char prompt[MAX_LINE];  // Buffer to store prompt 

    // Display the header at the start
    display_header();

    while (1) {
        display_prompt(prompt);  // Display custom prompt
        line = readline(prompt);  // Read input from user using readline

        // If user types "exit" or presses Ctrl+D, break the loop
        if (line == NULL || strcmp(line, "exit") == 0) {
            free(line);  // Free allocated line buffer
            break;
        }

        // Add command to history
        add_history(line);

        // Parse the input into command and arguments
        int i = 0;
        char *token = strtok(line, " ");  // Split input by spaces
        while (token != NULL) {
            args[i++] = token;  // Store each argument in args array
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the array for execvp

        // Create a child process to execute the command
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            continue;
        } else if (pid == 0) {
            // Child process: execute the command
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, ANSI_COLOR_RED 
                        "Error executing command '%s': %s\n" 
                        ANSI_COLOR_RESET, 
                        args[0], strerror(errno));  
                        // Print error if execvp fails
            }
            exit(EXIT_FAILURE);  // Exit child process if execvp fails
        } else {
            // Parent process: wait for the child process to complete and report status
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                printf("Command exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                int signal_num = WTERMSIG(status);
                if (signal_num == SIGSEGV) {
                    fprintf(stderr, ANSI_COLOR_RED 
                    "Oopsie!! Segmentation fault (signal %d) detected.\ncode dumped.\n" 
                    ANSI_COLOR_RESET, signal_num);
                } else {
                    printf("Command terminated by signal %d\n", signal_num);
                }
            }
        }
        free(line);  // Free allocated line buffer
    }

    printf("Exiting MyShell\n");
    return 0;  // Exit the shell
}
