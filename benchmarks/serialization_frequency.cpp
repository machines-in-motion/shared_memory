#include <chrono>
#include "shared_memory/demos/four_int_values.hpp"
#include "shared_memory/serializer.hpp"

// checks at which frequency 1 million items can be serialized

void execute()
{
    int nb_iterations = 1000000;  // 1 million

    shared_memory::Four_int_values fiv(1, 1, 1, 1);

    shared_memory::Serializer<shared_memory::Four_int_values> serializer;
    long int total_size_=0;

    auto start = std::chrono::steady_clock::now();

    for (int iteration = 0; iteration < nb_iterations; iteration++)
    {
        const std::string& serialized = serializer.serialize(fiv);
        // just to make sure the compiler does not remove the line
        // above for optimization purposes
        total_size_ += serialized.size();
    }

    auto end = std::chrono::steady_clock::now();

    long int duration_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start)
            .count();
    double duration_seconds = static_cast<double>(duration_us) / 1e6;

    double frequency = static_cast<double>(nb_iterations) / duration_seconds;

    std::cout << "\nserialization frequency: " << frequency
              << " | irrelevant data: " << total_size_ << "\n\n";
}

int main()
{
    execute();
    return 0;
}
