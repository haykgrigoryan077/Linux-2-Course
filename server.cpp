#include "common.h"
#include <iostream>
#include <queue>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

class ThreadPool
{
private:
  queue<Task *> taskQueue;
  pthread_t *threads;
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

  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1)
  {
    std::cerr << "Failed to create socket." << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string serverIP = "127.0.0.1";
  int serverPort = 8080;

  struct sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(serverPort);
  inet_pton(AF_INET, serverIP.c_str(), &(serverAddress.sin_addr));

  if (bind(serverSocket, (struct sockaddr *)(&serverAddress), sizeof(serverAddress)) == -1)
  {
    std::cerr << "Failed to bind the socket to the address." << std::endl;
    close(serverSocket);
    exit(EXIT_FAILURE);
  }

  if (listen(serverSocket, 5) == -1)
  {
    std::cerr << "Failed to listen on the socket." << std::endl;
    close(serverSocket);
    exit(EXIT_FAILURE);
  }

  while (true)
  {
    std::cout << "Server is listening for connections..." << std::endl;

    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(serverSocket, (struct sockaddr *)(&clientAddress), &clientAddressLength);
    if (clientSocket == -1)
    {
      std::cerr << "Failed to accept client connection." << std::endl;
      close(serverSocket);
      exit(EXIT_FAILURE);
    }

    std::cout << "Client connected: " << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << std::endl;

    while (true)
    {
      char buffer[1024];
      int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
      if (bytesRead == -1)
      {
        std::cerr << "Failed to receive data from client." << std::endl;
        close(clientSocket);
        break;
      }
      else if (bytesRead == 0)
      {
        std::cout << "Client disconnected." << std::endl;
        close(clientSocket);
        break;
      }

      buffer[bytesRead] = '\0';
      std::cout << "Incoming data: " << buffer << std::endl;

      std::string response = simple_tokenizer(std::string(buffer));

      if (send(clientSocket, response.c_str(), response.size(), 0) == -1)
      {
        std::cerr << "Failed to send response to client." << std::endl;
        close(clientSocket);
        break;
      }
    }
  }

  close(serverSocket);

  return 0;
}
