/**
 * @file set_data.cpp
 * @author Vincent Berenz
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief Create a small app that set the data into a shared memory. This
 * memory is read from the counter part of this app: get_data
 */

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "shared_memory/shared_memory.hpp"

static bool RUNNING = true;

// the shared memory does not keep track of what
// segment and objects it created (to be realtime compatible),
// so cleaning has to be "manual"

void cleaning_memory(int)
{
    RUNNING = false;
    shared_memory::clear_shared_memory("main_memory");
}

int main()
{
    cleaning_memory(0);
    RUNNING = true;

    // cleaning on ctrl+c
    struct sigaction cleaning;
    cleaning.sa_handler = cleaning_memory;
    sigemptyset(&cleaning.sa_mask);
    cleaning.sa_flags = 0;
    sigaction(SIGINT, &cleaning, nullptr);

    double d1 = 0.0;
    double d2 = 0.0;

    double v1[2];

    std::vector<double> v2(2, 0.0);

    Eigen::VectorXd v3 = Eigen::VectorXd(4);

    std::map<int, double> m1;
    m1[0] = d1;
    m1[1] = d2;

    std::map<std::string, double> m2;
    m2["value_1"] = d1;
    m2["value_2"] = d2;

    std::map<std::string, std::vector<double>> m3;
    m3["value_1"] = std::vector<double>(2, 0.0);
    m3["value_2"] = std::vector<double>(2, 0.0);

    std::map<std::string, Eigen::VectorXd> m4;
    m4["value_1"] = Eigen::VectorXd(4);
    m4["value_2"] = Eigen::VectorXd(4);

    const std::string s1("my string");

    unsigned count = 0;
    while (RUNNING)
    {
        d1 += 0.01;
        d2 += 0.001;

        int offset = 0;
        v1[0] = (d1 + offset);
        ++offset;
        v1[1] = (d1 + offset);
        ++offset;

        v2[0] = (d1 + offset);
        ++offset;
        v2[1] = (d1 + offset);
        ++offset;

        v3[0] = (d1 + offset);
        ++offset;
        v3[1] = (d1 + offset);
        ++offset;
        v3[2] = (d1 + offset);
        ++offset;
        v3[3] = (d1 + offset);
        ++offset;

        m1[0] = (d1 + offset);
        ++offset;
        m1[1] = (d1 + offset);
        ++offset;

        m2["value_1"] = (d1 + offset);
        ++offset;
        m2["value_2"] = (d1 + offset);
        ++offset;

        m3["value_1"][0] = (d1 + offset);
        ++offset;
        m3["value_2"][0] = (d1 + offset);
        ++offset;

        m3["value_1"][1] = (d1 + offset);
        ++offset;
        m3["value_2"][1] = (d1 + offset);
        ++offset;

        m4["value_1"][0] = (d1 + offset);
        ++offset;
        m4["value_1"][1] = (d1 + offset);
        ++offset;
        m4["value_1"][2] = (d1 + offset);
        ++offset;
        m4["value_1"][3] = (d1 + offset);
        ++offset;
        m4["value_2"][0] = (d1 + offset);
        ++offset;
        m4["value_2"][1] = (d1 + offset);
        ++offset;
        m4["value_2"][2] = (d1 + offset);
        ++offset;
        m4["value_2"][3] = (d1 + offset);
        ++offset;

        shared_memory::set("main_memory", "d1", d1);
        shared_memory::set("main_memory", "d2", d2);
        shared_memory::set("main_memory", "v1", v1, 2);
        shared_memory::set("main_memory", "v2", v2);
        shared_memory::set("main_memory", "v3", v3);
        shared_memory::set("main_memory", "m1", m1);
        shared_memory::set("main_memory", "m2", m2);
        shared_memory::set("main_memory", "m3", m3);
        shared_memory::set("main_memory", "m4", m4);
        shared_memory::set("main_memory", "s1", s1);
        // shared_memory::set("main_memory","protobuf_string",
        // arguments.SerializeAsString());

        ++count;
        std::cout << ".";
        if (count % 100 == 0)
        {
            std::cout << std::endl;
        }
        usleep(1000);
    }
}
