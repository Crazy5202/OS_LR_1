#include <unistd.h>
#include <sys/wait.h>
#include <string>

int main() {
    int pipe_fd[2]; // creating pipe to child process; pipe_fd[0] - for reading, pipe_fd[1] - for writing
    int pipe_bw[2]; // creating pipepipe from executed program to parent process
    if ((pipe(pipe_fd) == -1) or (pipe(pipe_bw)==-1)) { // creating pipes
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
        execlp("./divider", "./divider", NULL); // executing program to divide the numbers
        perror("Exec error");
        return 6;
    } else { // going in parent process
        close(pipe_fd[0]); // closing the descriptor of forward pipe to read
        close(pipe_bw[1]); // closing the descriptor of backward pipe to write
        std::string input = ""; // creating string for reading input
        int counter = 0; // creating variable for counting lines of input
        while(counter != 2) {
            char c;
            if (read(STDIN_FILENO, &c, sizeof(char)) == -1) { // reading from input
                perror("Input reading error");
                return 3;
            }
            if (c == '\n' and counter !=0) {
                input = "";
                counter += 1;
            } else if (c == ' ' or c == '\n') {
                if (write(pipe_fd[1], &input, sizeof(input)) == -1) { // writing to forward pipe
                    perror("Writing to forward pipe error");
                    return 4;
                }
                input="";
                if (counter == 0) {
                    counter += 1;
                }
            } else {
                input += c;
            }
        }
        if (input!="") {
            perror("Wrong input format");
            return 5;
        }
        close(pipe_fd[1]); // closing the descriptor to write because it is no longer needed 
        int status;
        wait(&status); // waiting for child process to finish
        if ((WIFEXITED(status)) and (WEXITSTATUS(status)!=0)) {
            perror("Child process had an error");
            return WEXITSTATUS(status);
        }
        read(pipe_bw[0], &input, sizeof(input));
        printf("Answer: %d\n", stoi(input));
    }
    return 0;
}