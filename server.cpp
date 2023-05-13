#include "common.h"

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

int main()
{

  sem_t *request_sem = get_semaphore(0, O_CREAT, 0644, 0);
  log_sem_value("after init request_sem", request_sem);
  sem_t *result_sem = get_semaphore(1, O_CREAT, 0644, 0);
  log_sem_value("after init result_sem", result_sem);

  if (request_sem == SEM_FAILED || result_sem == SEM_FAILED)
  {
    std::cerr << "Could not create semaphore\n";
    exit(EXIT_FAILURE);
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

  Task *shared_memory_pointer = static_cast<Task*>(mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (shared_memory_pointer == MAP_FAILED)
  {
    std::cerr << "Could not map the shared memory";
  }

  while (true)
  {
    std::cout << "working" << std::endl;
    if (sem_wait(request_sem) != 0)
    {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    std::cout << "incoming args: " << shared_memory_pointer->arg_1 << " "
              << shared_memory_pointer->arg_2 << " " << shared_memory_pointer->type << std::endl;
    sleep(1);
    if (shared_memory_pointer->type == ADD_ID)
    {
      std::cout << "add\n";
      shared_memory_pointer->result = get_sum(shared_memory_pointer->arg_1, shared_memory_pointer->arg_2);
    }
    else if (shared_memory_pointer->type == MINUS_ID)
    {
      shared_memory_pointer->result = get_minus(shared_memory_pointer->arg_1, shared_memory_pointer->arg_2);
    }
    else if (shared_memory_pointer->type == MULTIPLE_ID)
    {
      shared_memory_pointer->result = get_multiply(shared_memory_pointer->arg_1, shared_memory_pointer->arg_2);
    }
    else if (shared_memory_pointer->type == DIVIDE_ID)
    {
      shared_memory_pointer->result = get_divide(shared_memory_pointer->arg_1, shared_memory_pointer->arg_2);
    }
    else
    {
      std::cerr << "shared_memory_pointer[0] > 3 || shared_memory_pointer[0] < 0";
      break;
    }
    sem_post(result_sem);
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
