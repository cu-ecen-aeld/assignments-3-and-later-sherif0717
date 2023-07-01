#include "systemcalls.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#define STDOUT 1



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
    int ret;
    ret = system(cmd);
    if(ret == -1 || errno == ECHILD || errno == EINTR || errno == EINVAL)
        return false;
    else 
    return true;
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
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/
    int status; //ret;
    pid_t pid;

    pid = fork();

    if(pid == -1)
    {
        printf("@pid = -1\n");
        exit(EXIT_FAILURE);        
    }
    else if(pid == 0)
    {
        execv(command[0], command);
        exit(EXIT_FAILURE);
    }

    if(waitpid(pid, &status, 0) == -1)
        exit(EXIT_FAILURE);
        

    if(WIFEXITED(status))
    {
        int exit_status = WEXITSTATUS(status);
        if(exit_status)
            return false;
        else
            return true;
    }
     
    va_end(args);
    return true;


    
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
    int i, status;
    pid_t pid;

    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
    
    int pri_fd, sec_fd;
    /*open outputfile with pri_fd, primary file descriptor*/
    pri_fd = open(outputfile, O_CREAT | O_RDWR, 0666);// | S_IRUSR | S_IWUSR);

    /*create duplicate (secondary) file  descriptor for satndard output*/
    sec_fd = dup(STDOUT);
    
    /*create child process*/
    pid = fork();
    if(pid == 0)
    {
        /*
        redirect standard output to outputfile
        by duplicating the file descriptor onto
        the file descrioptor for standard output
        */
        dup2(pri_fd, STDOUT);

        /*close file descriptor for outputfile*/
        close(pri_fd);

        /*execute command to redirect into outputfile*/
        execv(command[0], command);

        /*restore original standard output file descriptor*/
        dup2(sec_fd, STDOUT);

        /*close original standard output file descriptor*/
        close(sec_fd);
    }

    /*wait for child process to stop or terminate*/
    if(waitpid(pid, &status, 0) == -1)
        exit(EXIT_FAILURE);

    /*was ending or terminating child process normal? */
    if(WIFEXITED(status))
    {
        /*get exit status of the child process*/
        int exit_status = WEXITSTATUS(status);
        if(exit_status)
            return false; //EXIT_FAILURE = 1
        else
            return true;   //EXIT_SUCCESS = 0
    }

    va_end(args);

    return true;
}
