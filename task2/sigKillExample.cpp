#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main() {
    pid_t pid = fork();
    
    if (pid == 0) {
        sleep(2);
        kill(getppid(), SIGKILL);
        std::cout << "Child process killed parent process\n";
    } else if (pid > 0) {
        std::cout << "Parent process created child process with PID " << pid << "\n";
        while (1); // for checking if the parent process killed or not
    } else {
        std::cerr << "Fork() failed\n";
        return 1;
    }
    
    return 0;
}
