This repository provides a solution to the bounded buffer problem, implemented using three mutexes: Mutex, Empty, and Full.
The bounded buffer problem is a classic synchronization problem where multiple producer threads and multiple consumer threads access a fixed-size buffer concurrently.
The Mutex, Empty, and Full mutexes are used to ensure thread safety and synchronization in this solution.
To address the bounded buffer problem, this solution employs three mutexes:
Mutex: The Mutex mutex is used to provide exclusive access to the shared buffer. Only one thread can acquire the Mutex mutex at a time, ensuring that only one thread can access the buffer at any given time.
Empty: The Empty mutex represents the number of empty slots available in the buffer. It helps control the insertion of items into the buffer. Producers wait on this mutex when the buffer is full, signaling them to wait until a slot becomes available.
Full: The Full mutex represents the number of occupied slots in the buffer. It helps control the retrieval of items from the buffer. Consumers wait on this mutex when the buffer is empty, signaling them to wait until an item becomes available.
This solution provides a comprehensive approach to solving the bounded buffer problem using three mutexes. By carefully coordinating the access to the shared buffer, it ensures that producers and consumers can work concurrently without interfering with each other and maintains the integrity of the buffer.
the polynomials input needs to be in this format:
(2:1,2,3)ADD(3:1,2,3,4)
(3:1,2,3,4)SUB(4:1,2,3,4,5)
(2:10,2,3)MUL(5:1,2,3,4,5,6)
when the first number is the polynomial degree.

