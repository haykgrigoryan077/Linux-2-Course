#include "common.h"
#include <iostream>
#include <queue>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

using namespace std;

class ThreadPool
{
private:
  queue<Task *> taskQueue;
  pthread_t * threads;
  int threadsCount;
  pthread_mutex_t *lock;
  pthread_cond_t *hasFunction;

public:
  ThreadPool(int threadsCount)
  {
    this->lock = new pthread_mutex_t();
    this->hasFunction = new pthread_cond_t();
    if (pthread_mutex_init(lock, nullptr) < 0)
    {
      perror("Couldn't init mutex");
    }

    if (pthread_cond_init(hasFunction, nullptr) < 0)
    {
      perror("Couldn't init cond_var");
    }
    this->threadsCount = threadsCount;
    this->threads = new pthread_t[this->threadsCount];
    for (int i = 0; i < this->threadsCount; i++)
    {
      pthread_create(&threads[i], nullptr, execute, this);
    }
  }

  ~ThreadPool()
  {
    pthread_cond_destroy(hasFunction);
    pthread_mutex_destroy(lock);
    for (int i = 0; i < this->threadsCount; i++)
    {
      pthread_kill(threads[i], SIGKILL);
    }
    delete[] threads;
  }

  void addTask(Task *task)
  {
    pthread_mutex_lock(lock);
    this->taskQueue.push(task);
    pthread_mutex_unlock(lock);
    pthread_cond_signal(hasFunction);
  }

  // void stop() {

  // }

  static void *execute(void *arg)
  {

    ThreadPool *threadpool = (ThreadPool *)arg;

    while (true)
    {
      pthread_mutex_lock(threadpool->lock);
      while (threadpool->taskQueue.empty())
      {
        pthread_cond_wait(threadpool->hasFunction, threadpool->lock);
      }
      Task *task = threadpool->taskQueue.front();
      threadpool->taskQueue.pop();
      pthread_mutex_unlock(threadpool->lock);
      task->execute_task();
    }
  }
};

int main()
{

  ThreadPool threadpool(5);

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

  Task *currentTask = static_cast<Task *>(mmap(NULL, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (currentTask == MAP_FAILED)
  {
    std::cerr << "Could not map the shared memory";
  }

  while (true)
  {
    std::cout << "server is working" << std::endl;
    if (sem_wait(request_sem) != 0)
    {
      perror("sem_wait");
      exit(EXIT_FAILURE);
    }
    std::cout << "incoming args: " << currentTask->arg_1 << " "
              << currentTask->arg_2 << " " << currentTask->type << std::endl;
    sleep(1);

    // currentTask->execute_task();

    threadpool.addTask(currentTask);

    sem_post(result_sem);
  }

  if (munmap(currentTask, SHARED_MEMORY_SIZE) == -1)
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