# Processes

## Task 1. (20 %)
Write a program, that creates n child processes. Number n is an argument given at the execution time. Every child process should print message about its PID.
## Task 2. (40%)
Write function mergeFiles - same as in the lab 1 but every merge ought to be performed by new child process. Compare times of executions for original mergeFiles function
and new mergeFiles function for large/medium/small number of files pairs and for large/medium/small number of rows in merged files.
## Task 3. (40%)
Write a program that takes as arguments - starting directory, phrase and depth. The program starts searching in given directory and looks for text files with given phrase. Every subdirectory is searched by new child process. The search does not goes deeper than specified depth. As a result, print PID of process that found a file and relative path from starting directory.
## Results
### Task 1,2,3 - complete
Makefile - commands:
- make compile - compiles a program
- make run - runs program with arguments specified in a Makefile
- make clean - remove all the unneccessary files

