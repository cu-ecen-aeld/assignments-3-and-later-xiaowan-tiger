#include "systemcalls.h"
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    int ret = system(cmd);
    if (ret == -1) {
        perror("system execute error");
        return false;
    } else {
        if (WIFEXITED(ret) && WEXITSTATUS(ret) == 0) {
            return true;
        } else {
            return false;
        }
    }
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    pid_t pid = fork();  // 创建子进程

    if (pid == -1) {
        // fork() 失败
        va_end(args);
        return false;
    } else if (pid == 0) {
        // 在子进程中执行 execv()
        execv(command[0], command);
        // 如果 execv() 执行失败，则终止子进程
        perror("execv");
        exit(EXIT_FAILURE);
    } else {
        // 父进程中等待子进程完成
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            va_end(args);
            return false;
        }

        // 检查子进程的退出状态
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            va_end(args);
            return true;
        } else {
            va_end(args);
            return false;
        }
    }
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    pid_t pid = fork();  // 创建子进程

    if (pid == -1) {
        // fork() 失败
        va_end(args);
        return false;
    } else if (pid == 0) {
        // 在子进程中执行文件重定向和 execv()

        // 打开输出文件
        int fd = open(outputfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        // 重定向标准输出到文件
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
            close(fd);
            exit(EXIT_FAILURE);
        }

        // 关闭文件描述符
        close(fd);

        // 执行 execv()
        execv(command[0], command);

        // 如果 execv() 执行失败，则终止子进程
        perror("execv");
        exit(EXIT_FAILURE);
    } else {
        // 父进程中等待子进程完成
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            va_end(args);
            return false;
        }

        // 检查子进程的退出状态
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            va_end(args);
            return true;
        } else {
            va_end(args);
            return false;
        }
    }

}
