#include "stdlib.h"
#include "stdio.h"
#include "unistd.h"
#include "fcntl.h"
#include "string.h"

int main() {
    int len;
    if (read(STDIN_FILENO, &len, sizeof(int)) == -1) { // reading length of data
        perror("Forward pipe reading error");
        _exit(-1);
    }
    char* data = malloc(sizeof(char)*len);
    if (read(STDIN_FILENO, data, len) == -1) { // reading whole data
        perror("Forward pipe reading error");
        _exit(-1);
    }
    char* var = strtok(data, "\n"); // getting filename
    var = strtok(NULL, "\n"); // getting array with all the numbers
    var = strtok(var, " "); // getting first number as char*
    if ((var == NULL) || (atoi(var)==0)) { // checking whether data is correct
        perror("data format error");
        _exit(-2);
    }
    int file = open(data, O_WRONLY | O_CREAT, 0777);
    if (file == -1) {
        perror("File reading error");
        _exit(-3);
    }
    int result=atoi(var); // getting first number as int
    var = strtok(NULL, " ");
    while (var != NULL) { // dividing
        printf("%d ", result);
        int del = atoi(var);
        if (del == 0) { // checking for 0 div
            perror("Zero divider error");
            _exit(-4);
        }
        result/=del;
        var = strtok(NULL, " ");
    }

    int num_len=1, num = result/10; // creating string version of result
    while (num>0) {
        num/=10;
        num_len++;
    }
    char res_str[num_len];
    sprintf(res_str, "%d", result);

    if ((write(file, "Here is the answer: ", 20*sizeof(char)) == -1) || (write(file, &res_str, num_len*sizeof(char)) == -1)) { // writing answer to the file
        perror("Writing to file error");
        _exit(-5);
    }
    if ((write(STDOUT_FILENO, &num_len, sizeof(int)) == -1) || (write(STDOUT_FILENO, &res_str, num_len*sizeof(char)) == -1)) { // writing to backward pipe
        perror("Writing to backward pipe error");
        _exit(-5);
    }
    return 0;
}