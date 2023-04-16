#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

int get_sum(int a, int b)
{
  return a + b;
}

int get_minus(int a, int b)
{
  return a - b;
}

int get_multiply(int a, int b)
{
  return a * b;
}

int get_divide(int a, int b)
{
  if (b == 0)
  {
    std::cerr << "Could not divide to 0";
    return 0;
  }
  return a / b;
}

int SHARED_MEMORY_SIZE = 4 * sizeof(int);
const char *SHARED_MEMORY_NAME = "/shared_memory";
const char *SEMAPHORE_NAME = "/my_semaphore";
const int ADD_ID = 0;
const int MINUS_ID = 1;
const int MULTIPLE_ID = 2;
const int DIVIDE_ID = 3; 

int main()
{

  sem_t *semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);

  if (semaphore == SEM_FAILED)
  {
    std::cerr << "Could not create semaphore";
  }

  int fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1)
  {
    std::cerr << "Could not open shared memory";
  }

  if (ftruncate(fd, SHARED_MEMORY_SIZE) == -1)
  {
    std::cerr << "Could not truncate the shared memory";
  }

  char *shared_memory_pointer = static_cast<char *>(mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory_pointer == MAP_FAILED)
  {
    std::cerr << "Could not map the shared memory";
  }

  while (true)
  {
    std::cout << "working";
    sem_wait(semaphore);
    if (shared_memory_pointer[0] == ADD_ID) {
      std::cout << "working";
      shared_memory_pointer[3] = get_sum(shared_memory_pointer[1], shared_memory_pointer[2]);
    } else if (shared_memory_pointer[0] == MINUS_ID) {
      shared_memory_pointer[3] = get_minus(shared_memory_pointer[1], shared_memory_pointer[2]);
    } else if (shared_memory_pointer[0] == MULTIPLE_ID) {
      shared_memory_pointer[3] = get_minus(shared_memory_pointer[1], shared_memory_pointer[2]);
    } else if (shared_memory_pointer[0] == DIVIDE_ID) {
      shared_memory_pointer[3] = get_divide(shared_memory_pointer[1], shared_memory_pointer[2]);
    } else {
      std::cerr << "shared_memory_pointer[0] > 3 || shared_memory_pointer[0] < 0";
      break;
    }
    sem_post(semaphore);
  }

  if (munmap(shared_memory_pointer, SHARED_MEMORY_SIZE) == -1)
  {
    std::cerr << "Could not map the shared memory";
    return 1;
  }

  if (shm_unlink(SHARED_MEMORY_NAME) == -1)
  {
    std::cerr << "Could not map the shared memory";
    return 1;
  }

  return 0;
}