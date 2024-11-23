#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Main program for process tree creation.
 *
 * This program creates child processes in a specific order, outputting
 * the process IDs and labels for each process, and waits for child
 * processes to complete as specified.
 *
 * @author: Tianyun Song
 * @version: October 31, 2024
 */
int main() {
    // Initial process A0
    printf("[%d] A0\n", getpid()); 

    // Fork to create process B1
    pid_t pidB1 = fork();
    if (pidB1 == 0) {
        // Child process B1
        printf("[%d] B1\n", getpid());
        printf("[%d] Bye\n", getpid());
    } else { 
        // A0-level process outputs B0 and waits for B1
        printf("[%d] B0\n", getpid());
        wait(NULL);  // Wait for B1 to finish

        printf("[%d] C0\n", getpid());
        printf("[%d] Bye\n", getpid());
        
        // Fork to create process C1
        pid_t pidC1 = fork();
        if (pidC1 == 0) {
            // Child process C1
            printf("[%d] C1\n", getpid());
            printf("[%d] Bye\n", getpid());
        } else {
            wait(NULL);  // Wait for C1 to finish
        }
    }
    return 0;
}
