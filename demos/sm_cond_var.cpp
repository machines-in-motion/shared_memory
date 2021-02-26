#include <unistd.h>
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "shared_memory/condition_variable.hpp"
#include "shared_memory/lock.hpp"
#include "shared_memory/mutex.hpp"

#define MUTEX_ID "sm_cond_var_demo_mutex"
#define CV_SEGMENT_ID "sm_cond_var_demo_segment"

class Config
{
public:
    int value;
    std::string message;
    std::atomic<bool> *running;
};

void *update_vector_async(void *config_)
{
    std::vector<int> v(1000000);
    shared_memory::Mutex mutex(MUTEX_ID);
    shared_memory::ConditionVariable condition(CV_SEGMENT_ID, true);

    Config *config = static_cast<Config *>(config_);

    while (*config->running)
    {
        mutex.lock();

        std::cout << config->message;

        for (int i = 0; i < v.size(); i++)
        {
            v[i] = config->value;
        }

        usleep(500);

        for (int i = 0; i < v.size(); i++)
        {
            if (v[i] != config->value)
            {
                std::cout << "--- damn, the vector was not locked !\n";
                break;
            }
            if (i < 10)
            {
                std::cout << " " << v[i];
            }
        }
        std::cout << "\n";

        mutex.unlock();
        condition.notify_one();

        usleep(10);
    }
}

void *update_vector(void *config_)
{
    Config *config = static_cast<Config *>(config_);

    std::vector<int> v(1000000);
    shared_memory::Mutex mutex(MUTEX_ID);
    shared_memory::ConditionVariable condition(CV_SEGMENT_ID, false);

    while (*config->running)
    {
        {
            shared_memory::Lock lock(mutex);
            condition.wait(lock);

            std::cout << config->message;
            for (int i = 0; i < v.size(); i++)
            {
                v[i] = config->value;
            }

            usleep(500);

            for (int i = 0; i < v.size(); i++)
            {
                if (v[i] != config->value)
                {
                    std::cout << "\n--- damn, the vector was not locked !\n";
                    break;
                }
                if (i < 10)
                {
                    std::cout << " " << v[i];
                }
            }
            std::cout << "\n";
        }

        condition.notify_one();
    }
}

int main()
{
    shared_memory::Mutex::clean(MUTEX_ID);

    std::atomic<bool> running(true);

    Config config1;
    config1.value = 1;
    config1.running = &running;
    config1.message = std::string("ping");
    std::thread thread1(update_vector, &config1);

    Config config2;
    config2.value = 2;
    config2.running = &running;
    config2.message = std::string("pong");
    std::thread thread2(update_vector, &config2);

    Config config3;
    config3.value = 3;
    config3.running = &running;
    config3.message = std::string("pung");
    std::thread thread3(update_vector_async, &config3);

    usleep(5000000);

    std::cout << "\tSTOPPING!\n";

    running = false;
    thread1.join();
    thread2.join();
    thread3.join();
}
