#include "common.h"

int main() {

  sem_t *request_sem = get_semaphore(0);
  sem_t *result_sem = get_semaphore(1);

  if (request_sem == SEM_FAILED || result_sem == SEM_FAILED)
  {
    std::cerr << "Could not create semaphore\n";
    exit(EXIT_FAILURE);
  }

  int fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    std::cerr << "Could not open shared memory";
  }

  if(ftruncate(fd, SHARED_MEMORY_SIZE) == -1) {
    std::cerr << "Could not truncate the memory";
  }

  Task* shared_memory_pointer = static_cast<Task*>(mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory_pointer == MAP_FAILED) {
    std::cerr << "Could not map the shared memory";
  }

  std::cout << "What you want to do?" << std::endl << "Here are variants" << std::endl << "0: ADD" << std::endl << "1: MINUS" << std::endl << "2: MULTIPLE" << std::endl << "3: DIVISION" << std::endl;

  int userDescisionID;
  std::cin >> userDescisionID;

  int firstNumber, secondNumber;
  std::cout << "Please enter numbers" << std::endl;
  std::cin >> firstNumber >> secondNumber;

  shared_memory_pointer->type = userDescisionID;
  shared_memory_pointer->arg_1 = firstNumber;
  shared_memory_pointer->arg_2 = secondNumber;

  sem_post(request_sem);
  
  sem_wait(result_sem);

  int result = shared_memory_pointer->result;
  std::cout << "The result is " << result << std::endl;

  if (munmap(shared_memory_pointer, SHARED_MEMORY_SIZE) == -1) {
    std::cerr << "Could not unmap the shared memory";
    return 1;
  }

  if (shm_unlink(SHARED_MEMORY_NAME) == -1) {
    std::cerr << "Could not unlink the shared memory";
    return 1;
  }

  return 0;
}
