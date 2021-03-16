# Memory managment, libraries and time measurements

## Task 1. (50%)
Design and prepare set of functions (library) for managment of array of blocks containing merged files - row by row (round robin). In the main array, first element is a pointer to first merged block, which stores pointers to rows of outcome of merging first two files.

The library must enable:
- creating array of pointers (main array)  
- defining and merging sequance of files - outcome should be stored n temporary file
- creating a merged block in main array from merged file
- returning number of rows in given merged block
- removing from memory a block given by index in main array
- removing from memory a row with a given index from given block
- printing all merged blocks

Prepare Makefile for both static and shared library compilation.

## Task 2. (25%)

Write a basic test using a library. The commands to execute should be given as program input.
E.g. ./main create_table size merge_files file1.txt:file2.txt create_block pair remove_block index remove_row row_index block_index print_blocks

Measure performance of given operations for small, medium and large number of pairs and rows, results print on console and save to file.
- merge files
- creating blocks
- removing blocks
- alternating operation of creating and removing block given number of times

For all operation measure three values: real time, user time and system time.

## Task 3. (50%)
(25%) Create Makefile, containing three options of running test from task 2:
- with static library
- with shared library
- with dynamic library

Save results to a file.

(25%) Add to the Makefile possibility of running tests with different optimization level — -O0…-Os. Results save to a file.
