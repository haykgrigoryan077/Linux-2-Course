#include <iostream>
#include <queue>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"

using namespace std;

class ThreadPool {
	private:
		queue<Task*> taskQueue;
		vector<pthread_t*> threads;
		int threadsCount;
		pthread_mutex_t* lock;
		pthread_cond_t* hasFunction;
	public:
		ThreadPool(int threadsCount) {
			if(pthread_mutex_init(lock, nullptr) < 0) {
				perror("Couldn't init mutex");
			}

			if(pthread_cond_init(hasFunction, nullptr) < 0) {
				perror("Couldn't init cond_var");
			}
			
			for (int i = 0; i < this-> threadsCount; i++) {
				pthread_create(this->threads[i], nullptr, execute, this);
			}
		}

		~ThreadPool() {
			pthread_cond_destroy(hasFunction);
			pthread_mutex_destroy(lock);
			for (int i = 0; i < this->threadsCount; i++){
				pthread_kill(*threads[i], SIGKILL);
			}
		} 
		

		void addTask(Task* task) {
			pthread_mutex_lock(lock);
			this->taskQueue.push(task);
			pthread_mutex_unlock(lock);
			pthread_cond_signal(hasFunction);
		}

		// void stop() {
		
		// }

	static void* execute(void* arg) {
			ThreadPool* threadpool = (ThreadPool*) arg;
			
			while(true) {
				pthread_mutex_lock(threadpool->lock);
				while(threadpool->taskQueue.empty()) {
					pthread_cond_wait(threadpool->hasFunction, threadpool->lock);
				}
				Task* task = threadpool->taskQueue.front();
				threadpool->taskQueue.pop();
				pthread_mutex_unlock(threadpool->lock);
				task->execute_task();
			}

		}
	
};

int main() {

	return 0;
}