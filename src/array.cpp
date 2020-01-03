#include "shared_memory/array.hpp"

namespace shared_memory
{

  void clear_array(std::string segment_id)
  {
    boost::interprocess::shared_memory_object::remove(segment_id.c_str());
    boost::interprocess::named_mutex::remove((segment_id+std::string("_mutex")).c_str());
  }

}
