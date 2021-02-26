#include <unistd.h>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class Config
{
public:
    std::vector<int> *vector;
    std::atomic<bool> *running;
    int value;
    std::string message;
    std::condition_variable *condition;
    std::mutex *mutex;
};

void *update_vector_async(void *config_)
{
    Config *config = static_cast<Config *>(config_);

    while (*config->running)
    {
        config->mutex->lock();

        std::cout << config->message;

        for (unsigned int i = 0; i < config->vector->size(); i++)
        {
            (*config->vector)[i] = config->value;
        }

        usleep(500);

        for (unsigned int i = 0; i < config->vector->size(); i++)
        {
            if ((*config->vector)[i] != config->value)
            {
                std::cout << "--- damn, the vector was not locked !\n";
                break;
            }
            if (i < 10)
            {
                std::cout << " " << (*config->vector)[i];
            }
        }
        std::cout << "\n";

        config->mutex->unlock();

        usleep(10);
    }

    return nullptr;
}

void *update_vector(void *config_)
{
    Config *config = static_cast<Config *>(config_);

    while (*config->running)
    {
        {
            std::unique_lock<std::mutex> lock(*config->mutex);
            config->condition->wait(lock);

            std::cout << config->message;
            for (unsigned int i = 0; i < config->vector->size(); i++)
            {
                (*config->vector)[i] = config->value;
            }

            usleep(500);

            for (unsigned int i = 0; i < config->vector->size(); i++)
            {
                if ((*config->vector)[i] != config->value)
                {
                    std::cout << "\n--- damn, the vector was not locked !\n";
                    break;
                }
                if (i < 10)
                {
                    std::cout << " " << (*config->vector)[i];
                }
            }
            std::cout << "\n";
        }

        config->condition->notify_one();
    }

    return nullptr;
}

int main()
{
    std::vector<int> v(1000000);
    std::atomic<bool> running(true);
    std::condition_variable condition;
    std::mutex mutex;

    Config config1;
    config1.value = 1;
    config1.vector = &v;
    config1.running = &running;
    config1.mutex = &mutex;
    config1.message = std::string("ping");
    config1.condition = &condition;
    std::thread thread1(update_vector, &config1);

    Config config2;
    config2.value = 2;
    config2.vector = &v;
    config2.running = &running;
    config2.mutex = &mutex;
    config2.condition = &condition;
    config2.message = std::string("pong");
    std::thread thread2(update_vector, &config2);

    Config config3;
    config3.value = 3;
    config3.vector = &v;
    config3.running = &running;
    config3.mutex = &mutex;
    config3.message = std::string("pung");
    std::thread thread3(update_vector_async, &config3);

    usleep(1000);
    condition.notify_one();
    usleep(10000000);

    std::cout << "\tSTOPPING!\n";

    running = false;
    thread1.join();
    thread2.join();
    thread3.join();
}
