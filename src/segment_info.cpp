// Copyright 2019 @ Max Planck Gesellschaft and New York University
// License BSD-3-Clause

#include "shared_memory/segment_info.hpp"

namespace shared_memory
{
// dev note : the boost api does not allow for msm to be const
SegmentInfo::SegmentInfo(boost::interprocess::managed_shared_memory &msm)
{
    size_ = msm.get_size();
    free_memory_ = msm.get_free_memory();
    has_issues_ = !msm.check_sanity();
    // -1 : there is one default object per segment.
    // it is clearer to returns the number of objects created
    // by users
    nb_objects_ = msm.get_num_named_objects() - 1;
}

uint SegmentInfo::get_size() const
{
    return size_;
}

uint SegmentInfo::get_free_memory() const
{
    return free_memory_;
}

bool SegmentInfo::has_issues() const
{
    return has_issues_;
}

uint SegmentInfo::nb_objects() const
{
    return nb_objects_;
}

void SegmentInfo::print() const
{
    std::cout << "Memory segment:"
              << "\n"
              << "\tsize:\t" << size_ << "\n"
              << "\tfree:\t" << free_memory_ << "\n"
              << "\tused:\t" << size_ - free_memory_ << "\n"
              << "\tobjects\t" << nb_objects_ << "\n"
              << "\thas issues:\t" << has_issues_ << std::endl;
}

}  // namespace shared_memory
