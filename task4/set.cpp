#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

using namespace std;

const int SHM_SIZE = 1024;
const char* SHARED_MEMORY = "/myshm";

int main() {
    int fd;
    char *shm_ptr;
    
    fd = shm_open(SHARED_MEMORY, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Error");
        exit(1);
    }
    
    if (ftruncate(fd, SHM_SIZE) == -1) {
        perror("Error");
        exit(1);
    }
    
    shm_ptr = static_cast<char *>(mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (shm_ptr == MAP_FAILED) {
        perror("Error");
        exit(1);
    }
    
    char buffer[SHM_SIZE];
    while (fgets(buffer, SHM_SIZE, stdin) != nullptr) {
        if (strlen(buffer) >= SHM_SIZE) {
            cerr << "Input string is too long for shared memory" << endl;
            exit(1);
        }
        
        strcpy(shm_ptr, buffer);
        shm_ptr[strlen(buffer)] = '\0';
        
        cout << "Wrote to shared memory => " << shm_ptr << endl;
    }
    
    munmap(shm_ptr, SHM_SIZE);
    shm_unlink(SHARED_MEMORY);
    
    return 0;
}

