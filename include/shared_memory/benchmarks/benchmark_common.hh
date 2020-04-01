/**
 * @file benchmark_common.hh
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Common tools for benchmarking
 */
#ifndef BENCHMARK_COMMON_HH
#define BENCHMARK_COMMON_HH

#include <signal.h>
#include <unistd.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#define SHARED_MEMORY_SIZE 65536
#define SIZE 1000
#define NUMBER_OR_MEASURED_ITERATIONS 1000
#define MAX_NUNMBER_OF_ITERATION 10000

typedef std::chrono::high_resolution_clock::time_point TimeType;

static std::vector<double> DATA(SIZE, 2);
bool RUNNING;
static std::string SHM_NAME("stress_test");
static std::string SHM_OBJECT_NAME("stress_object");

struct MeasureTime
{
    MeasureTime()
    {
        start();
    }
    void start()
    {
        tic_ = std::chrono::high_resolution_clock::now();
    }
    void update()
    {
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                         tic_ - prev_tic_)
                         .count();
        frequency_ = 1000.0 / (pow(10.0, -9.0) * nanos);
        period_ = 1.0 / frequency_;
        prev_tic_ = tic_;
        tic_ = std::chrono::high_resolution_clock::now();
    }

    friend std::ostream& operator<<(std::ostream& os, const MeasureTime& dt);

    TimeType tic_;
    TimeType prev_tic_;
    double frequency_;
    double period_;
};

std::ostream& operator<<(std::ostream& os, const MeasureTime& time)
{
    os << "Frequency = " << time.frequency_ << " ; "
       << "Period = " << time.period_;
    return os;
}

void init_benchmark();

void code_to_benchamrk();

void end_benchmark();

#endif  // BENCHMARK_COMMON_HH
