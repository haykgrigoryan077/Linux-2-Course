#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) == -1) {
        cerr << "Pipe creation failed" << endl;
        exit(1);
    }

    pid1 = fork();
    if (pid1 == -1) {
        cerr << "Fork failed" << endl;
        exit(1);
    } else if (pid1 == 0) {
        close(fd[0]);
        int mypid = getpid();
        write(fd[1], &mypid, sizeof(mypid));
        close(fd[1]);
        exit(0);
    }

    pid2 = fork();
    if (pid2 == -1) {
        cerr << "Fork failed" << endl;
        exit(1);
    } else if (pid2 == 0) {
        close(fd[1]);
        int firstProcessID;
        read(fd[0], &firstProcessID, sizeof(firstProcessID));
        cout << "Process 2 received ID of process 1: " << firstProcessID << endl;
        close(fd[0]);
        exit(0);
    }

    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    exit(0);
}
