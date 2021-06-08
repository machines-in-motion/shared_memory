[![continuous integration](https://raw.githubusercontent.com/MPI-IS-BambooAgent/sw_badges/master/badges/plans/corerobotics/tag.svg?sanitize=true)](url)
[![check_main_branch_and_deploy_doc](https://github.com/machines-in-motion/shared_memory/actions/workflows/deploy_doc.yml/badge.svg)](https://github.com/machines-in-motion/shared_memory/actions/workflows/deploy_doc.yml)

Readme
------

### what it is

* This library is a convenient wrapper over boost:interprocess. It is meant to be trivial to use, and to make interprocess communication between processes trivial
* It is meant to be realtime friendly (no dynamic allocation)

### how it works / API

An example covering the complete API is in [src/examples](https://git-amd.tuebingen.mpg.de/amd-clmc/shared_memory/tree/master/src/examples) . You may run the related executables.

### how to run the unit tests

```
colcon build
```

This will *not* work ('install' is required) :

```
catkin run_tests
```

### Limitations

* There must be in the code a "manual" cleanup of the memory. See for example: [set_data](https://git-amd.tuebingen.mpg.de/amd-clmc/shared_memory/blob/master/src/examples/set_data.cpp#L16)

* Arrays, map and vectors are expected to be of a predefined fixed size. Even when using the 'get' methods, they are expected to have been declared with the correct size (and with the correct set of keys for maps)

### Removing shared memory file

On linux, the shared memory mappings are created under `/dev/shm`. If necessary (e.g. if a program was creating the shared memory with the wrong permissions), these files can be delted manually.

