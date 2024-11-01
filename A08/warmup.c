#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("[%d] A0\n", getpid());  // Process A0
    
    pid_t pidB1 = fork();
    if (pidB1 == 0) {  // Child B1
        printf("[%d] B1\n", getpid());
        printf("[%d] Bye\n", getpid());
    } else { // A0 level
        printf("[%d] B0\n", getpid()); // B0
        wait(NULL);  // Wait for B1 to finish
        printf("[%d] C0\n", getpid());
        printf("[%d] Bye\n", getpid());
        
        pid_t pidC1 = fork();
        if (pidC1 == 0) {  // Child C1
            printf("[%d] C1\n", getpid());
            printf("[%d] Bye\n", getpid());
        } else {
            wait(NULL);  // Wait for C1 to finish
        }
    }
    return 0;
}
