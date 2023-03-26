#include <iostream>
#include <csignal>
#include <unistd.h>

int sigusr2_count = 0;

void sigint_handler(int signum) {
    std::cout << "Total number of SIGUSR2 signals received: " << sigusr2_count << std::endl;
    exit(0);
}

void sigusr2_handler(int signum) {
    sigusr2_count++;
}

int main() {
    std::signal(SIGINT, sigint_handler);
    std::signal(SIGUSR1, SIG_IGN);
    std::signal(SIGUSR2, sigusr2_handler);
    
    while (1) {
        std::cout << "I am still alive" << std::endl;
        sleep(5);
    }
}

