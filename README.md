# Shared Memory and Semaphores in C

A C program that demonstrates inter-process communication using shared memory and semaphores. The program includes functionality for creating and managing shared memory segments, using semaphores for synchronization, and performing operations on shared data structures.

---

**Description:**

This program showcases the usage of shared memory and semaphores in C for inter-process communication. It includes functionality for initializing shared memory segments, managing access to shared resources using semaphores, and performing operations based on user input.

---

**Project Structure:**

1. **`main.c`:**
   - Contains the main logic for creating shared memory segments, initializing semaphores, and implementing inter-process communication using semaphores and shared memory.

---

**Dependencies:**
- This program requires the `sys/ipc.h`, `sys/shm.h`, `fcntl.h`, `semaphore.h`, `pthread.h`, `stdio.h`, and `stdlib.h` header files for system calls, shared memory operations, semaphore handling, thread management, and standard I/O operations.

---

**Usage:**

1. Compile the program using a C compiler (e.g., `gcc -o main main.c -lpthread`).
2. Run the compiled executable (`./main`) to execute the shared memory and semaphore demonstration.
3. Follow the on-screen prompts to interact with the shared memory segments, perform operations, and observe the synchronization using semaphores.

---

**Bounded Buffer Problem:**

This repository provides a solution to the bounded buffer problem, implemented using three mutexes: Mutex, Empty, and Full. The bounded buffer problem is a classic synchronization problem where multiple producer threads and multiple consumer threads access a fixed-size buffer concurrently. The Mutex, Empty, and Full mutexes are used to ensure thread safety and synchronization in this solution.

- Mutex: Provides exclusive access to the shared buffer. Only one thread can acquire the Mutex mutex at a time, ensuring that only one thread can access the buffer at any given time.
- Empty: Represents the number of empty slots available in the buffer. Producers wait on this mutex when the buffer is full, signaling them to wait until a slot becomes available.
- Full: Represents the number of occupied slots in the buffer. Consumers wait on this mutex when the buffer is empty, signaling them to wait until an item becomes available.

This solution offers a comprehensive approach to solving the bounded buffer problem using three mutexes, ensuring concurrent operation of producers and consumers without interference and maintaining buffer integrity.

---

**Polynomials Input Format:**

The input for polynomials should be in the following format:
```
(2:1,2,3)ADD(3:1,2,3,4) (3:1,2,3,4)SUB(4:1,2,3,4,5) (2:10,2,3)MUL(5:1,2,3,4,5,6)
```
Where the first number represents the polynomial degree followed by its coefficients. Operations between polynomials are denoted by `ADD`, `SUB`, and `MUL`.
