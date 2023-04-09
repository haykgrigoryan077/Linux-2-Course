#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <vector>

using namespace std;

int is_prime(int n) {
    if (n <= 1) {
        return 0;
    }
    for (int i = 2; i*i <= n; i++) {
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}

int main() {
    int p[2];
    pid_t pid;
    char command_line_input[10];
    int m;

    if (pipe(p) < 0) {
        cerr << "Pipe error\n";
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        cerr << "Fork error\n";
        return 1;
    }

    if (pid == 0) {
        close(p[1]);
        int n, count = 0;
        read(p[0], &n, sizeof(n));
        for (int i = 2; ; i++) {
            if (is_prime(i)) {
                count++;
                if (count == n) {
                    write(p[0], &i, sizeof(i));
                    break;
                }
            }
        }
        close(p[0]);
        return 0;
    } else {
        close(p[0]);
        while (true) {
            cout << "Enter a number or 'exit': ";
            cin >> command_line_input;
            if (strcmp(command_line_input, "exit") == 0) {
                break;
            }
            m = atoi(command_line_input);
            write(p[1], &m, sizeof(m));
            int prime;
            read(p[1], &prime, sizeof(prime));
            cout << "The " << m << "-th prime number is " << prime << endl;
        }
        close(p[1]);
        wait(NULL);
    }

    return 0;
}

