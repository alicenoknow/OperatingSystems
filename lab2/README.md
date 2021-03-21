# Files management

## All tasks must be performed in two ways:
- using system functions (open(), write() etc.)
- using library functions (fopen(), fwrite() etc.)
## For both implementations for every task, measure times of execution and save it in appropiate file.

## Task 1. (20 %)
Write a program, that opens two files. Filenames can be specified as program arguments but if none were given user should be asked to type them in. Program should print lines from both files alternately until the end of a longer file.
## Task 2. (20%)
Write a program, that takes two arguments. First is a character and second is a filename. Program should display in a console only lines which contain specified character.
## Task 3. (20%)
Given a file with random numbers:
- save to file a.txt number of even numbers
- save to file b.txt all numbers which have tens digit equal to either 0 or 7
- save to file c.txt all numbers which are a square of an integer
## Task 4. (20%)
Write a program, which takes arguments input filename, output filename and two strings. Rewrite lines from input file to output file but replace all lines equal to first string with second string.
## Task 5. (20%)
Write a program that given input filename and output filename rewrites lines from input file to output file but breaks lines that are longer than 50 characters.
## Results
### Task 1 - complete
Makefile - commands:
- make compile - compiles a program
- make run1 - runs program with arguments specified in a Makefile
- make run2 - runs program and waits for arguments to be typed by user
- make clean - remove all the unneccessary files
### Task 2, 3, 4, 5 - complete
Makefile - commands:
- make compile - compiles a program
- make run - runs program with arguments specified in a Makefile
- make clean - remove all the unneccessary files
