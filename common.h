#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cerrno>
#include <cstdlib>
#include <limits.h>

int SHARED_MEMORY_SIZE = 4 * sizeof(int);
const char *SHARED_MEMORY_NAME = "shared_memory";
const char* SEMAPHORE_NAMES[2] = {"request_semaphore", "result_semaphore"};
const int ADD_ID = 0;
const int MINUS_ID = 1;
const int MULTIPLE_ID = 2;
const int DIVIDE_ID = 3;

using sem_id = int;
sem_t* get_semaphore(const sem_id sem_type, int flag = 0, int mode = 0, int value = 0)
{
   sem_t *semaphore = sem_open(SEMAPHORE_NAMES[sem_type], flag, mode, value);

   if (semaphore == SEM_FAILED)
   {
      perror("sem_open");
      std::cerr << "semaphore name: " << SEMAPHORE_NAMES[sem_type] << " flag: " << flag << " mode: " << mode << " value: " << value << "\n";
   }
   return semaphore;
}

void log_sem_value(const char* msg, sem_t* sem)
{
   int val = INT_MIN;
   if (sem_getvalue(sem, &val) != 0)
   {
      perror("sem_getvalue");
   }

   std::cout << msg << " sem_value: " << val << std::endl;
}
