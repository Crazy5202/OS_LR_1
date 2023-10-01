#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <iostream>

int main() {
    std::string parcel;
    read(STDIN_FILENO, &parcel, sizeof(parcel));
    std::cout << parcel << '\n';
    int file = open(parcel.c_str(), O_WRONLY);
    if (file == -1) {
        return -1;
    }
    read(STDIN_FILENO, &parcel, sizeof(parcel));
    int result = std::stoi(parcel);
    while (read(STDIN_FILENO, &parcel, sizeof(parcel)) != 0) {
        int ddr = std::stoi(parcel);
        if (ddr==0) {
            return -2;
        }
        result/=ddr;
    }
    printf("%d\n", result);
    std::string res_str = std::to_string(result);
    if (write(file, &res_str, sizeof(result)) == -1) { // writing answer to the file
        return -3;
    }
    if (write(STDOUT_FILENO, &res_str, sizeof(res_str)) == -1) { // writing to backward pipe
        return -4;
    }
    return 0;
}