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

#define MAX_LINE 1024 // Maximum length of input line / 输入行的最大长度

// ANSI color codes
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


// Function to display the initial header with colorful lines and "Rainbow shell" message
void display_header() {
    printf(ANSI_COLOR_CYAN "<3 Rainbow shell <3" ANSI_COLOR_RESET "\n"
           ANSI_COLOR_RED "--------" ANSI_COLOR_GREEN "--------" ANSI_COLOR_YELLOW "--------" ANSI_COLOR_BLUE "--------\n" ANSI_COLOR_RESET
           ANSI_COLOR_MAGENTA "--------" ANSI_COLOR_CYAN "--------" ANSI_COLOR_RED "--------" ANSI_COLOR_GREEN "--------\n" ANSI_COLOR_RESET);
}

// Function to display a custom prompt with colorful user, host, and cwd info
void display_prompt(char *prompt) {
    char cwd[MAX_LINE];
    struct passwd *pw = getpwuid(geteuid());  // Get username / 获取用户名
    char hostname[MAX_LINE];

    // Get current working directory and hostname / 获取当前目录和主机名
    if (getcwd(cwd, sizeof(cwd)) != NULL && gethostname(hostname, sizeof(hostname)) == 0) {
        // Format the prompt with user@host:cwd and colorful <3 ----- / 使用彩色 user@host:cwd 提示符
        snprintf(prompt, MAX_LINE,
                 ANSI_COLOR_RED "%.10s" ANSI_COLOR_RESET
                 "@" ANSI_COLOR_GREEN "%.10s" ANSI_COLOR_RESET
                 ":" ANSI_COLOR_YELLOW "%.50s" ANSI_COLOR_RESET
                 " " ANSI_COLOR_BLUE "<3 " ANSI_COLOR_MAGENTA "-----" ANSI_COLOR_CYAN " -----" ANSI_COLOR_RESET " ",
                 pw->pw_name, hostname, cwd);
    } else {
        perror("Error getting prompt information");
        snprintf(prompt, MAX_LINE, "MyShell> ");
    }
}

int main() {
    char *line;  // Pointer to store user input / 用于存储用户输入的指针
    char *args[MAX_LINE / 2 + 1];  // Array to store command arguments / 存储命令参数的数组
    char prompt[MAX_LINE];  // Buffer to store prompt / 存储提示符的缓冲区

    // Display the initial header only once at the start / 仅在启动时显示一次 header
    display_header();

    while (1) {
        display_prompt(prompt);  // Display custom prompt / 显示自定义提示符
        line = readline(prompt);  // Read input from user using readline / 使用 readline 获取用户输入

        // If user types "exit" or presses Ctrl+D, break the loop / 如果用户输入 "exit" 或按下 Ctrl+D，退出循环
        if (line == NULL || strcmp(line, "exit") == 0) {
            free(line);  // Free allocated line buffer / 释放 line 缓冲区
            break;
        }

        // Add command to history / 将命令添加到历史记录
        add_history(line);

        // Parse the input into command and arguments / 将输入解析为命令和参数
        int i = 0;
        char *token = strtok(line, " ");  // Split input by spaces / 按空格分割输入
        while (token != NULL) {
            args[i++] = token;  // Store each argument in args array / 将每个参数存储在 args 数组中
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the array for execvp / 使用 NULL 终止数组，以便 execvp 使用

        // Create a child process to execute the command / 创建子进程来执行命令
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            continue;
        } else if (pid == 0) {
            // Child process: execute the command / 子进程：执行命令
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, ANSI_COLOR_RED "Error executing command '%s': %s\n" ANSI_COLOR_RESET, 
                        args[0], strerror(errno));  // Print error if execvp fails / 如果 execvp 执行失败则打印错误
            }
            exit(EXIT_FAILURE);  // Exit child process if execvp fails / 如果 execvp 失败则退出子进程
        } else {
            // Parent process: wait for the child process to complete and report status / 父进程：等待子进程完成并报告状态
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                printf("Command exited with status %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                int signal_num = WTERMSIG(status);
                if (signal_num == SIGSEGV) {
                    fprintf(stderr, ANSI_COLOR_RED "Oopsie!! Segmentation fault (signal %d) detected.\ncode dumped.\n" ANSI_COLOR_RESET, signal_num);
                } else {
                    printf("Command terminated by signal %d\n", signal_num);
                }
            }
        }
        free(line);  // Free allocated line buffer / 释放 line 缓冲区
    }

    printf("Exiting MyShell\n");
    return 0;  // Exit the shell / 退出 Shell
}

