# Operating Systems Assignment 4: Producer-Consumer and Reader-Writer Locks

This repository contains implementations for two synchronization problems in C as part of the MTL458 Operating Systems course assignment:

1. **Single Producer-Single Consumer** using mutex locks and condition variables.
2. **Reader-Writer Locks** with both reader-preference and writer-preference versions.

## Table of Contents
- [Objective](#objective)
- [Part 1: Producer-Consumer Problem](#part-1-producer-consumer-problem)
- [Part 2: Reader-Writer Locks](#part-2-reader-writer-locks)

## Objective

This project implements solutions for two classic concurrency problems in operating systems: the Single Producer-Single Consumer problem and Reader-Writer locks with both reader-preference and writer-preference mechanisms. The implementations are in C, utilizing mutex locks, condition variables, and semaphores.

## Part 1: Producer-Consumer Problem

This program simulates a producer-consumer scenario where:
- The **shared resource** is a circular buffer queue of size 100, holding unsigned integers.
- The **producer** reads integers from an input file (`input-part1.txt`) and adds them to the buffer. It pauses if the buffer is full.
- The **consumer** removes items from the buffer and pauses if it is empty.

When the producer reads a `0`, it stops processing, and the program terminates.

### Input/Output
- **Input File:** `input-part1.txt` - integers, one per line; ends with 0 to signal termination.
- **Output File:** `output-part1.txt` - logs each consumed item and the buffer state.


### Compilation and Execution

To compile and run the producer-consumer solution:

```bash
gcc prod-cons.c -o prod-cons -lpthread
./prod-cons
```


## Part 2: Reader-Writer Locks

This part implements two types of reader-writer locks:

- **Reader-Preference Lock:** Prioritizes readers, allowing them to continue accessing the data even if a writer is waiting, which may lead to writer starvation.
- **Writer-Preference Lock:** Ensures that no new readers can acquire the lock if a writer is waiting, preventing writer starvation.

- Uses semaphores for synchronization.
- Logs activity to separate output files for each version.

### Input/Output
- **Input/Output File:** shared-file.txt - used by readers and writers.
- **Output Files:**
 - `output-reader-pref.txt` for reader-preference lock.
 - `output-writer-pref.txt` for writer-preference lock.


### Compilation and Execution

To compile and run each lock type:

For reader-preference:
```bash
gcc rwlock-reader-pref.c -o reader-pref -lpthread
./reader-pref <number_of_readers> <number_of_writers>
```

For writer-preference:
```bash
gcc rwlock-writer-pref.c -o writer-pref -lpthread
./writer-pref <number_of_readers> <number_of_writers>
```

