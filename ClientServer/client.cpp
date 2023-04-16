#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

int SHARED_MEMORY_SIZE = 4 * sizeof(int);
const char* SHARED_MEMORY_NAME = "/shared_memory";
const char* SEMAPHORE_NAME = "/my_semaphore";

int main() {

  sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 0);

  if (semaphore == SEM_FAILED) {
    std::cerr << "Could not create semaphore";
  }

  int fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    std::cerr << "Could not open shared memory";
  }


  if(ftruncate(fd, SHARED_MEMORY_SIZE) == -1) {
    std::cerr << "Could not truncate the memory";
  }

  int* shared_memory_pointer = static_cast<int*>(mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory_pointer == MAP_FAILED) {
    std::cerr << "Could not map the shared memory";
  }

  std::cout << "What you want to do?" << std::endl << "Here are variants" << std::endl << "0: ADD" << std::endl << "1: MINUS" << std::endl << "2: MULTIPLE" << std::endl << "3: DIVISION" << std::endl;

  int userDescisionID;
  std::cin >> userDescisionID;

  int firstNumber, secondNumber;
  std::cout << "Please enter numbers" << std::endl;
  std::cin >> firstNumber >> secondNumber;

  shared_memory_pointer[0] = userDescisionID;
  shared_memory_pointer[1] = firstNumber;
  shared_memory_pointer[2] = secondNumber;

  sem_post(semaphore);

  sem_wait(semaphore);

  sleep(5);

  int result = shared_memory_pointer[3];
  std::cout << "The result is " << result << std::endl;

  if (munmap(shared_memory_pointer, SHARED_MEMORY_SIZE) == -1) {
    std::cerr << "Could not unmap the shared memory";
    return 1;
  }

  if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
    std::cerr << "Could not unlink the shared memory";
    return 1;
  }

  sem_close(semaphore);

  return 0;
}