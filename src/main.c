#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main() {
    int pipe_fd[2]; // creating pipe to child process; pipe_fd[0] - for reading, pipe_fd[1] - for writing
    int pipe_bw[2]; // creating pipe from executed program to parent process
    if ((pipe(pipe_fd) == -1) || (pipe(pipe_bw)==-1)) { // creating pipes
        perror("Pipe creation error");
        return 1;
    }
    pid_t pid = fork(); // creating child process
    if (pid == -1) {
        perror("Child process creation error");
        return 2;
    } else if (pid == 0) { // going in child process
        close(pipe_fd[1]); // closing the descriptor of forward pipe to write
        close(pipe_bw[0]); // closing the descriptor of backward pipe to read
        dup2(pipe_fd[0], STDIN_FILENO); // duplicating descriptors
        dup2(pipe_bw[1], STDOUT_FILENO);
        execl("divider", "divider", NULL); // executing program to divide the numbers
        perror("Exec error");
        return 3;
    } else { // going in parent process
        close(pipe_fd[0]); // closing the descriptor of forward pipe to read
        close(pipe_bw[1]); // closing the descriptor of backward pipe to write
        char* input = 0; // creating char* pointer for reading input
        int len = 0;
        int counter = 0; // creating variable for counting lines of input
        while(counter != 2) {
            char c;
            if (read(STDIN_FILENO, &c, sizeof(char)) == -1) { // reading from input
                perror("Input reading error");
                return 4;
            }
            input = realloc(input, sizeof(char)*(++len));
            input[len-1]=c;
            if (c == '\n') {
                counter += 1;
            }
        }
        input[len-1]=' '; // doing this for strtok later
        input = realloc(input, sizeof(char)*(++len));
        input[len-1]='\0';
        if ((write(pipe_fd[1], &len, sizeof(int)) == -1) || (write(pipe_fd[1], input, sizeof(char)*len) == -1)) { // writing to forward pipe
            perror("Writing to forward pipe error");
            return 5;
        }
        close(pipe_fd[1]); // closing the descriptor to write because it is no longer needed 
        
        int status;
        wait(&status); // waiting for child process to finish
        if (WIFEXITED(status) && WEXITSTATUS(status)!=0) { // checking for error in child
            return 6;
        }
        int answ_len;
        
        if (read(pipe_bw[0], &answ_len, sizeof(int)) == -1) { // reading answer length from pipe
            perror("Backward pipe reading error");
            return 4;
        }
        char* answer = malloc(sizeof(char)*answ_len);
        if (read(pipe_bw[0], &answer, answ_len*sizeof(char)) == -1) { // reading answer from pipe
            perror("Backward pipe reading error");
            return 4;
        }
        if ((write(STDOUT_FILENO, "Here is the answer: ", 20*sizeof(char)) == -1) || (write(STDOUT_FILENO, &answer, answ_len*sizeof(char)) == -1) || (write(STDOUT_FILENO, "\n", sizeof(char)) == -1)) { // writing answer to output
            perror("Writing to stdout error");
            return 5;
        }
    }
    return 0;
}