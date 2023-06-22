# Operating Systems

This repository contains all ten assignments for the Operating Systems course at AGH University of Science and Technology. The main aim of this course was to introduce students to the mechanics used in Unix-like operating systems, using the C language. Each task received the maximum number of points.  

### Index

- [`lab1` - Memory management, libraries and time measurements](cw01)
- [`lab2` - File system, operations on files](cw02)
- [`lab3` - Create processes, process environment and process controll (*forks*)](cw03)
- [`lab4` - IPC - Signals in Unix systems](cw04)
- [`lab5` - IPC - Pipes in Unix systems](cw05)
- [`lab6` - IPC - Message queues](cw06)
- [`lab7` - IPC - Shared memory and semaphores](cw07)
- [`lab8` - Threads](cw08)
- [`lab9` - Thread synchronization methods - Mutexes](cw09)
- [`lab10` - Sockets](cw10)

### Requirements
- gcc 
- make
- Unix-like operating system (preferred Linux)

### Running

To compile each program, use the following command in its directory::

```bash
make all
```

To remove executable files (.exe), object files (.o), library files (.a) etc. from the program directory, use the following command:

```bash
make clean
```
