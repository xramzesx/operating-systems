# Operating Systems

This repository contains all ten assignments for the Operating Systems course at AGH University of Science and Technology. The main aim of this course was to introduce students to the mechanics used in Unix-like operating systems, using the C language. Each task received the maximum number of points.  

### Index

- [`lab1` - Memory management, libraries and time measurements](lab1)
- [`lab2` - File system, operations on files](lab2)
- [`lab3` - Create processes, process environment and process controll (*forks*)](lab3)
- [`lab4` - IPC - Signals in Unix systems](lab4)
- [`lab5` - IPC - Pipes in Unix systems](lab5)
- [`lab6` - IPC - Message queues](lab6)
- [`lab7` - IPC - Shared memory and semaphores](lab7)
- [`lab8` - Threads](lab8)
- [`lab9` - Thread synchronization methods - Mutexes](lab9)
- [`lab10` - Sockets](lab10)

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
