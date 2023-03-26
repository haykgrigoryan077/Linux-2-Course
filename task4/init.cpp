#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    const char *file_name = "shared_file";
    const size_t shm_size = 1024;
    int fd;
    void *shm_ptr;
    int shm_id;

    fd = open(file_name, O_CREAT | O_RDWR, S_IRWXU);
    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    if (ftruncate(fd, shm_size) == -1) {
        perror("Failed to truncate file");
        exit(1);
    }

    shm_id = shm_open("shared_mem", O_CREAT | O_RDWR, S_IRWXU);
    if (shm_id == -1) {
        perror("Failed to create shared memory");
        exit(1);
    }

    if (ftruncate(shm_id, shm_size) == -1) {
        perror("Failed to truncate shared memory");
        exit(1);
    }

    shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Failed to mmap shared memory");
        exit(1);
    }

    return 0;
}
