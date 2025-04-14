/*
 * MyShell - 一个简单的Shell实现
 * 作者：刘鑫宇
 * 学号：23371510
 * 日期：2025-04-14
 * 功能：
 *   1. 运行不带参数的外部命令
 *   2. 支持标准I/O重定向
 *   3. 支持管道连接两个外部命令
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>

#define MAX_CMD_LEN 1024   // 最大命令长度
#define MAX_ARGS 20        // 最大参数数量
#define PROMPT "myshell> " // 命令提示符

volatile sig_atomic_t sigint_received = 0;
// 解析输入的命令行
int parse_command(char *cmdline, char **args) {
    int i = 0;
    char *token = strtok(cmdline, " \t\n");
    
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[i] = NULL; // 参数列表以NULL结尾
    
    return i;
}

// 执行简单的命令（无重定向和管道）
void execute_simple_command(char **args) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed"); // fork出错
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // 子进程
        execvp(args[0], args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else { // 父进程
        waitpid(pid, NULL, 0);
    }
}

// 处理输入重定向
void handle_input_redirect(char **args, char *filename) {
    int fd = open(filename, O_RDONLY);
    int saved_stdin = dup(STDIN_FILENO);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    
    dup2(fd, STDIN_FILENO);
    close(fd);
    
    execute_simple_command(args);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
}

// 处理输出重定向（覆盖）
void handle_output_redirect(char **args, char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int saved_stdout = dup(STDOUT_FILENO);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    
    dup2(fd, STDOUT_FILENO);
    close(fd);
    
    execute_simple_command(args);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}

// 处理输出重定向（追加）
void handle_output_append(char **args, char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    int saved_stdout = dup(STDOUT_FILENO);
    if (fd < 0) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }
    
    dup2(fd, STDOUT_FILENO);
    close(fd);
    
    execute_simple_command(args);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
}

// 处理管道命令
void handle_pipe_command(char **args1, char **args2) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid1 == 0) { // 第一个命令（写端）
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        
        execvp(args1[0], args1);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid2 == 0) { // 第二个命令（读端）
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        
        execvp(args2[0], args2);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }
    
    close(pipefd[0]);
    close(pipefd[1]);
    
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void sigint_handler(int sig) {
    sigint_received = 1;
    write(STDOUT_FILENO, "\n" PROMPT, strlen("\n" PROMPT));
    tcflush(STDIN_FILENO, TCIFLUSH);
    fflush(stdout);
}

// 主函数
int main() {
    signal(SIGINT, sigint_handler);
    char cmdline[MAX_CMD_LEN];
    char *args[MAX_ARGS];
    char *args2[MAX_ARGS];
    
    printf("欢迎使用MyShell\n");
    
    while (1) {
        printf(PROMPT);
	fflush(stdout);
        if (fgets(cmdline, sizeof(cmdline), stdin) == NULL) {
            printf("\n");
            break; // 处理Ctrl+D
        } else if (sigint_received) {
	    // 如果是被 SIGINT 中断，继续循环
            clearerr(stdin);  // 清除 stdin 的错误状态
	    sigint_received = 0;
            continue;
        }
        
        if (strcmp(cmdline, "\n") == 0) {
            continue; // 空行
        }
        
        // 检查管道
        char *pipe_pos = strchr(cmdline, '|');
        if (pipe_pos != NULL) {
            *pipe_pos = '\0';
            char *cmd2 = pipe_pos + 1;
            
            int num_args1 = parse_command(cmdline, args);
            int num_args2 = parse_command(cmd2, args2);
            
            if (num_args1 > 0 && num_args2 > 0) {
                handle_pipe_command(args, args2);
            } else {
                printf("管道命令格式错误\n");
            }
            continue;
        }
        
        // 检查重定向
        int num_args = parse_command(cmdline, args);
        if (num_args == 0) continue;
        
        bool has_redirect = false;
        
        for (int i = 0; args[i] != NULL; i++) {
            if (strcmp(args[i], "<") == 0) { // 输入重定向
                if (args[i+1] == NULL) {
                    printf("缺少输入文件\n");
                    has_redirect = true;
                    break;
                }
                args[i] = NULL;
                handle_input_redirect(args, args[i+1]);
                has_redirect = true;
                break;
            } else if (strcmp(args[i], ">") == 0) { // 输出重定向（覆盖）
                if (args[i+1] == NULL) {
                    printf("缺少输出文件\n");
                    has_redirect = true;
                    break;
                }
                args[i] = NULL;
                handle_output_redirect(args, args[i+1]);
                has_redirect = true;
                break;
            } else if (strcmp(args[i], ">>") == 0) { // 输出重定向（追加）
                if (args[i+1] == NULL) {
                    printf("缺少输出文件\n");
                    has_redirect = true;
                    break;
                }
                args[i] = NULL;
                handle_output_append(args, args[i+1]);
                has_redirect = true;
                break;
            }
        }
        
        if (!has_redirect && num_args > 0) {
            if (strcmp(args[0], "exit") == 0) {
                break; // 退出shell
            }
            execute_simple_command(args);
        }
    }
    
    printf("感谢使用MyShell，再见！\n");
    return 0;
}
