/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"

using namespace std;

void ThreadPool::dispatcher() {
    while (true) {
        // Esperar a que llegue una tarea
        tasks_to_dispatch_semaphore.wait();
        if (shutting_down) break;

        // Esperar a que un worker este disponible
        available_workers_semaphore.wait();
        if (shutting_down) break;

        tasks_queue_lock.lock();
        function<void(void)> thunk = tasks_queue.front(); // Obtener la primera tarea de la cola
        tasks_queue.pop();
        tasks_queue_lock.unlock();

        available_workers_lock.lock();
        size_t worker_id = available_workers_queue.front(); // Obtener el ID del primer worker disponible
        available_workers_queue.pop();
        available_workers_lock.unlock();

        // Asignarle su tarea (thunk) y despertar al worker específico.
        wts[worker_id].thunk = thunk;
        wts[worker_id].work_ready_semaphore.signal();
    }
}

void ThreadPool::worker(size_t id) {
    while (true) {
        // Esperar hasta que el dispatcher le asigne una tarea
        wts[id].work_ready_semaphore.wait();
        if (shutting_down) break;

        // Ejecutar la tarea (thunk) asignada 
        wts[id].thunk();

        available_workers_lock.lock();
        available_workers_queue.push(id); // Una vez terminada la tarea agregarlo devuelta a la cola de espera
        available_workers_lock.unlock();
        available_workers_semaphore.signal(); // Avisar al dispatcher que hay un nuevo worker dispoible 

        completion_lock.lock();
        tasks_completed++;
        if (tasks_completed == total_tasks_scheduled) {
            all_tasks_done_cv.notify_all(); // Notificar a todos los que esperan en wait()
        }
        completion_lock.unlock();
    }
}

ThreadPool::ThreadPool(size_t numThreads) :
    wts(numThreads),
    shutting_down(false),
    tasks_to_dispatch_semaphore(0),
    available_workers_semaphore(numThreads),
    total_tasks_scheduled(0),
    tasks_completed(0) {

    // Lanzar el hilo dispatcher.
    dt = thread(&ThreadPool::dispatcher, this);

    // Lanzar todos los hilos workers.
    for (size_t i = 0; i < numThreads; ++i) {
        wts[i].ts = thread(&ThreadPool::worker, this, i);
        available_workers_queue.push(i); // Encolar su ID
    }
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (shutting_down) return; // No agregar mas tareas en caso de estar apagandose

    completion_lock.lock();
    total_tasks_scheduled++;
    completion_lock.unlock();

    tasks_queue_lock.lock();
    tasks_queue.push(thunk);
    tasks_queue_lock.unlock();
    
    // Avisarle al dispatcher que hay una nueva tarea disponible
    tasks_to_dispatch_semaphore.signal();
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(completion_lock);
    // Bloquea hasta que la cantidad de tareas asignadas sea igual a la cantidad de tareas completadas
    all_tasks_done_cv.wait(lock, [this] {
        return tasks_completed == total_tasks_scheduled;
    });
}

ThreadPool::~ThreadPool() {
    // Esperar a que terminen todas las tareas pendientes
    wait();

    shutting_down = true;

    // Despertar a todos los hilos para que actualizen el flag y salgan del bucle
    tasks_to_dispatch_semaphore.signal();
    available_workers_semaphore.signal();
    
    for (size_t i = 0; i < wts.size(); ++i) {
        wts[i].work_ready_semaphore.signal();
    }
    // Esperamos a que todos los hilos (tanto dispatcher como workers) hallan salido de su bucle
    dt.join();
    for (size_t i = 0; i < wts.size(); ++i) {
        wts[i].ts.join();
    }
}
