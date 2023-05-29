#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cerrno>
#include <cstdlib>
#include <limits.h>

const int ADD_ID = 0;
const int MINUS_ID = 1;
const int MULTIPLE_ID = 2;
const int DIVIDE_ID = 3;

class Task
{
public:
   int arg_1, arg_2, type, result;
   Task(int arg_1, int arg_2, int type)
   {
      this->arg_1 = arg_1;
      this->arg_2 = arg_2;
      this->type = type;
   }

   void execute_task()
   {
      switch (this->type)
      {
      case ADD_ID:
         std::cout << "add";
         result = this->arg_1 + this->arg_2;
         break;
      case MINUS_ID:
         std::cout << "working";
         result = this->arg_1 - this->arg_2;
         break;
      case MULTIPLE_ID:
         result = this->arg_1 * this->arg_2;
         break;
      case DIVIDE_ID:

         std::cout << "divide";

         if (this->arg_2 == 0)
         {
            exit(EXIT_FAILURE);
         }
         result = this->arg_1 / this->arg_2;
         break;
      default:
         break;
      }
   }
};

int SHARED_MEMORY_SIZE = 4 * sizeof(Task *);
const char *SHARED_MEMORY_NAME = "shared_memory";
const char *SEMAPHORE_NAMES[2] = {"request_semaphore", "result_semaphore"};

using sem_id = int;
sem_t *get_semaphore(const sem_id sem_type, int flag = 0, int mode = 0, int value = 0)
{
   sem_t *semaphore = sem_open(SEMAPHORE_NAMES[sem_type], flag, mode, value);

   if (semaphore == SEM_FAILED)
   {
      perror("sem_open");
      std::cerr << "semaphore name: " << SEMAPHORE_NAMES[sem_type] << " flag: " << flag << " mode: " << mode << " value: " << value << "\n";
   }
   return semaphore;
}

void log_sem_value(const char *msg, sem_t *sem)
{
   int val = INT_MIN;
   if (sem_getvalue(sem, &val) != 0)
   {
      perror("sem_getvalue");
   }

   std::cout << msg << " sem_value: " << val << std::endl;
}
