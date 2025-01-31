#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>

class ThreadPool
{
    public:
        ThreadPool(size_t num_threads) : stop(false)
        {
            //Inicializar el threadpool con el numero de threads especificado
            for(size_t i = 0; i < num_threads; ++i)
            {
                threads.emplace_back([this] {
                    while(true)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock<std::mutex> lock(queue_mutex);
                            //Esperar a que haya una tarea en la cola
                            condition.wait(lock, [this] { return stop || tasks.empty(); });
                            if(stop && tasks.empty())
                            {
                                return;
                            }
                            task = std::move(tasks.front());
                            tasks.pop();
                        }

                        //ejecutar la tarea
                        task();
                    }
                });
            }
        }

        //Agregar una tarea al threadpool
        template <class F>
        void enqueue(F&& f)
        {        
            {   
                std::unique_lock<std::mutex> lock(queue_mutex);
                tasks.emplace(std::forward<F>(f));
            }
            condition.notify_one(); //Notificar a un thread que hay una tarea nueva
        }

        //Detener el threadpool
        void shutdown()
        {
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                stop = true; //Indicar que se debe detener el threadpool
            }
            condition.notify_all(); //Notificar a todos los hilos que se debe detener

            for(std::thread& thread : threads)
            {
                thread.join();
            }
        }

    private:
        std::vector<std::thread> threads; //contenedor de threads
        std::queue<std::function<void()>> tasks; //cola de tareas
        std::mutex queue_mutex; //Mutex para proteger la cola de tareas
        std::condition_variable condition; //Condicion para notificar a los threads
        bool stop; // Indicador para detener el threadpool
};

int main()
{
    ThreadPool pool(4); //Inicializar un threadpool con 4 threads

    //Agregar tareas al threadpool
    pool.enqueue([] { std::cout<<"Tarea 1"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 2"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 3"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 4"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 5"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 6"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 7"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 8"<<std::endl; });
    pool.enqueue([] { std::cout<<"Tarea 9"<<std::endl; });

    //Detener el threadpool
    pool.shutdown();
}