// Copyright 2019 Max Planck Gesellschaft and New York University
// Authors : Vincent Berenz, Maximilien Naveau

#include "shared_memory/locked_condition_variable.hpp"
#include "shared_memory/condition_variable.hpp"
#include "shared_memory/lock.hpp"
#include "shared_memory/mutex.hpp"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace shared_memory;

PYBIND11_MODULE(shared_memory_py,m){


  // Mutex
  
  pybind11::class_<Mutex>(m,"Mutex")
    .def(pybind11::init<std::string,bool>())
    .def("unlock",&Mutex::unlock)
    .def("lock",&Mutex::lock)
    .def_static("clean",&Mutex::clean);

  // Lock
  
  pybind11::class_<Lock>(m,"Lock")
    .def(pybind11::init<Mutex&>());

  // ConditionVariable
  
  pybind11::class_<ConditionVariable>(m,"ConditionVariable")
    .def(pybind11::init<std::string,bool>())
    .def("notify_all",&ConditionVariable::notify_all)
    .def("notify_one",&ConditionVariable::notify_one)
    .def("timed_wait",&ConditionVariable::timed_wait)
    .def("wait",&ConditionVariable::wait)
    .def_static("clean",&ConditionVariable::clean);

  // LockedConditionVariable
  
  pybind11::class_<LockedConditionVariable>(m,"LockedConditionVariable")
    .def(pybind11::init<std::string,std::string,bool>())
    .def("notify_all",&LockedConditionVariable::notify_all)
    .def("notify_one",&LockedConditionVariable::notify_one)
    .def("timed_wait",&LockedConditionVariable::timed_wait)
    .def("wait",&LockedConditionVariable::wait)
    .def("try_lock",&LockedConditionVariable::try_lock)
    .def("unlock",&LockedConditionVariable::unlock)
    .def("lock_scope",&LockedConditionVariable::lock_scope)
    .def("unlock_scope",&LockedConditionVariable::unlock_scope)
    .def_static("clean",&LockedConditionVariable::clean);


  // Shared memory

  // sharing a boolean
  m.def("set_bool",[](std::string segment,
		      std::string object,
		      bool value){
	  shared_memory::set<bool>(segment,object,value);
	});
  m.def("get_bool",[](std::string segment,
		      std::string object){
	  bool value;
	  shared_memory::get<bool>(segment,object,value);
	  return value;
	});

  
  
  
}



  
