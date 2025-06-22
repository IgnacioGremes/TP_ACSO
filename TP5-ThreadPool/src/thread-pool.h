/**
 * File: thread-pool.h
 * -------------------
 * This class defines the ThreadPool class, which accepts a collection
 * of thunks (which are zero-argument functions that don't return a value)
 * and schedules them in a FIFO manner to be executed by a constant number
 * of child threads that exist solely to invoke previously scheduled thunks.
 */

#ifndef _thread_pool_
#define _thread_pool_

#include <cstddef>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Semaphore.h"

using namespace std;

/**
 * @brief Represents a worker in the thread pool.
 * 
 * The `worker_t` struct contains information about a worker 
 * thread in the thread pool. Should be includes the thread object, 
 * availability status, the task to be executed, and a semaphore 
 * (or condition variable) to signal when work is ready for the 
 * worker to process.
 */
typedef struct worker {
    thread ts;
    function<void(void)> thunk;
    Semaphore work_ready_semaphore;
} worker_t;

class ThreadPool {
public:
  /**
  * Constructs a ThreadPool configured to spawn up to the specified
  * number of threads.
  */
    ThreadPool(size_t numThreads);

  /**
  * Schedules the provided thunk (which is something that can
  * be invoked as a zero-argument function without a return value)
  * to be executed by one of the ThreadPool's threads as soon as
  * all previously scheduled thunks have been handled.
  */
    void schedule(const function<void(void)>& thunk);

  /**
  * Blocks and waits until all previously scheduled thunks
  * have been executed in full.
  */
    void wait();

  /**
  * Waits for all previously scheduled thunks to execute, and then
  * properly brings down the ThreadPool and any resources tapped
  * over the course of its lifetime.
  */
    ~ThreadPool();

private:
    void worker(size_t id);
    void dispatcher();

    vector<worker_t> wts; // Contenedor de los workers
    bool shutting_down; // Flag para la secuencia de apagado

    Semaphore tasks_to_dispatch_semaphore; // El dispatcher espera en este si no hay tareas
    Semaphore available_workers_semaphore; // El dispatcher espera en este si no hay workers libres

    size_t total_tasks_scheduled;
    size_t tasks_completed;


    thread dt;

    queue<function<void(void)>> tasks_queue; // Cola de thunks
    mutex tasks_queue_lock; // Mutex para proteger la cola de tareas

    queue<size_t> available_workers_queue; // Cola de IDs de workers disponibles
    mutex available_workers_lock; // Mutex para proteger la cola de workers

    mutex completion_lock;
    condition_variable all_tasks_done_cv;

    // Evitar que el pool se pueda clonar
    ThreadPool(const ThreadPool& original) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
};

#endif
