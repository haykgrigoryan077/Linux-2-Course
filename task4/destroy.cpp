#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>

using namespace std;

int main() {
    int shm_id;

    shm_id = shm_open("shared_mem", O_RDWR, S_IRWXU);
    if (shm_id == -1) {
        perror("Failed to open shared memory");
        exit(1);
    }

    if (shm_unlink("shared_mem") == -1) {
        perror("Failed to destroy shared memory");
        exit(1);
    }

    return 0;
}
