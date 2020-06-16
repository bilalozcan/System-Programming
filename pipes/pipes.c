/* "$ ls -1 | cut -d'.' -f1 | sort | uniq | tee dirents" terminal komutunun
 * C programında exec ve pipe sistem çağrıları ile gerçekleştirilmesi*/

#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
    int ret;
    int pipe_fds[2];
    pipe2(pipe_fds, O_CLOEXEC); /* Call with the O_CLOEXEC flag to prevent any commands from blocking */
    pid_t pid = fork();
    if (pid == 0){ //CHILD
        dup2(pipe_fds[1], 1); //Write pipe
        ret = execlp("ls", "ls", "-1", (char *)NULL);
        if (ret == -1) //execlp failure
            perror("execlp");
    }else if(pid > 0){ //PARENT
        int pipe_fds2[2];
        pipe2(pipe_fds2, O_CLOEXEC); /* Call with the O_CLOEXEC flag to prevent any commands from blocking */
        pid_t pid2 = fork();
        if(pid2 == 0){ //CHILD
            dup2(pipe_fds[0], 0); // read pipe
            dup2(pipe_fds2[1], 1); // write pipe2
            ret = execlp("cut", "cut", "-f1", "-d.", (char *)NULL);
            if (ret == -1) //execlp failure
                perror("execlp");
        }else if(pid2 > 0){ //PARENT
            int pipe_fds3[2];
            pipe2(pipe_fds3, O_CLOEXEC); /* Call with the O_CLOEXEC flag to prevent any commands from blocking */
            pid_t pid3 = fork();
            if(pid3 == 0){ //CHILD
                dup2(pipe_fds2[0], 0); // read pipe2
                dup2(pipe_fds3[1], 1); // write pipe3
                ret = execlp("sort", "sort", (char *)NULL);
                if (ret == -1) //execlp failure
                    perror("execlp");
            }else if(pid3 > 0){ //PARENT
                int pipe_fds4[2];
                pipe2(pipe_fds4, O_CLOEXEC); /* Call with the O_CLOEXEC flag to prevent any commands from blocking */
                pid_t pid4 = fork();
                if(pid4 == 0){ //CHILD
                    dup2(pipe_fds3[0], 0); // read pipe3
                    dup2(pipe_fds4[1], 1); // write pipe4
                    ret= execlp("uniq", "uniq", (char *)NULL);
                    if (ret == -1) //execlp failure
                        perror("execlp");
                }else if(pid4 > 0){ //PARENT
                    dup2(pipe_fds4[0], 0); //read pipe4
                    ret = execlp("tee", "tee", "dirents", (char *)NULL);
                    if (ret == -1) //execlp failure
                        perror("execlp");
                }else if (pid4 == -1) //fork failure
                    perror ("fork");
            }else if (pid3 == -1) //fork failure
                perror ("fork");
        }else if (pid2 == -1) //fork failure
            perror ("fork");
    }else if (pid == -1) //fork failure
        perror ("fork");

    return 0;
}
