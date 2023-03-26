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


//#include <iostream>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//
//using namespace std;
//
//// function to check if a number is prime
//bool is_prime(int n) {
//    if (n <= 1) {
//        return false;
//    }
//    for (int i = 2; i * i <= n; i++) {
//        if (n % i == 0) {
//            return false;
//        }
//    }
//    return true;
//}
//
//// function to find the nth prime number
//int find_nth_prime(int n) {
//    int count = 2;
//    int i = 2;
//    while (count < n) {
//        if (is_prime(count)) {
//            count++;
//        }
//        i++;
//    }
//    return count;
//}
//
//int main() {
//    int fd[2];
//    pid_t pid;
//    int m, nth_prime;
//
//    // create pipe
//    if (pipe(fd) == -1) {
//        cerr << "Error: pipe creation failed" << endl;
//        exit(1);
//    }
//
//    pid = fork();
//    if (pid == -1) {
//        cerr << "Error: fork failed" << endl;
//        exit(1);
//    } else if (pid == 0) {
//        int n;
//        while (true) {
//            read(fd[0], &n, sizeof(n));
//            if (n == -1) {
//                break;
//            }
//            int nth_prime = find_nth_prime(n);
//            write(fd[1], &nth_prime, sizeof(nth_prime));
//        }
//        close(fd[0]); // close read end of pipe
//        exit(0);
//    }
//
//    // parent process
//    close(fd[0]); // close read end of pipe
//    fd_set read_fds;
//    while (true) {
//        // read input from user
//        cout << "Enter a positive integer (or 'exit' to quit): ";
//        string input;
//        cin >> input;
//        if (input == "exit") {
//            // send exit command to child process and exit
//            int exit_command = -1;
//            write(fd[1], &exit_command, sizeof(exit_command));
//            break;
//        } else {
//            // convert input to integer and send to child process
//            m = stoi(input);
//            write(fd[1], &m, sizeof(m));
//
//            // wait for data to be available in pipe
//            FD_ZERO(&read_fds);
//            FD_SET(fd[0], &read_fds);
//            int retval = select(fd[0] + 1, &read_fds, NULL, NULL, NULL);
//            if (retval == -1) {
//                cerr << "Error: select failed" << endl;
//                exit(1);
//            } else if (retval > 0 && FD_ISSET(fd[0], &read_fds)) {
//                // read result from child process
//                read(fd[0], &nth_prime, sizeof(nth_prime));
//                // print result
//                cout << "The " << m << "-th prime number is " << nth_prime << endl;
//            } else {
//                cerr << "Error: no data available in pipe" << endl;
//                exit(1);
//            }
//        }
//    }
//    close(fd[1]); // close write end of pipe
//    waitpid(pid, NULL, 0);
//    exit(0);
//}
