/**
 * @file shared_memory.hpp
 * @author Vincent Berenz
 * @author Maximilien Naveau (maximilien.naveau@gmail.com)
 * @license License BSD-3-Clause
 * @copyright Copyright (c) 2019, New York University and Max Planck
 * Gesellschaft.
 * @date 2019-05-22
 *
 * @brief This file declares a class that is used for shared memory segment
 * introspection (e.g. used and free memory)
 */

#pragma once

#include <boost/interprocess/managed_shared_memory.hpp>
#include <iostream>

namespace shared_memory
{
/*! @brief encapsulate information related to a shared memory segment */
class SegmentInfo
{
public:
    /*! @brief introspection of the shared memory segment */
    SegmentInfo(boost::interprocess::managed_shared_memory &msm);

    /*! @brief total size of the segment */
    uint get_size() const;

    /*! @brief free memory of the segment */
    uint get_free_memory() const;

    /*! @brief used memory of the segment */
    uint get_used_memory() const;

    /*! @brief report on the status of the internal structures of the segment */
    bool has_issues() const;

    /*! @brief number of objects allocated in the segment*/
    uint nb_objects() const;

    /*! @brief print in the terminal informations about the segment*/
    void print() const;

private:
    uint size_;
    uint free_memory_;
    bool has_issues_;
    uint nb_objects_;
};

}  // namespace shared_memory
