
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <fcntl.h>

#include <semaphore.h>

#define MAX_INPUT_LENGTH 129 // 128+ /n
#define MAX_BUFFER_SIZE 1291 // 1280 + /n * 10 polynomials + one /0

int main() {
    sem_t * mutex, * full, * empty;
    sem_unlink("/my_mutex1");
    sem_unlink("/my_mutex2");
    sem_unlink("/my_mutex3");
    // the mutex for entering the critical section,runs from 1(open) to 0(close)
    mutex = sem_open("/my_mutex1", O_CREAT, 0666, 1);
    if (mutex == SEM_FAILED) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    // the full sem tells how much full cells there are in the shared memory
    full = sem_open("/my_mutex2", O_CREAT, 0666, 0);
    if (full == SEM_FAILED) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    // the empty sem tells how much empty cells there are in the shared memory
    empty = sem_open("/my_mutex3", O_CREAT, 0666, 10);
    if (empty == SEM_FAILED) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    key_t key;
    key = ftok("/tmp", 'z');
    if (key == -1) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    int shm_id = shmget(key, MAX_BUFFER_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id == -1) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    char * shm_ptr; // a pointer to the start of the shared memory
    shm_ptr = (char * ) shmat(shm_id, NULL, 0);
    if (shm_ptr == (char * ) - 1) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    int index = 0;
    int end = 1;
    while (1) {
        char input[MAX_INPUT_LENGTH + 1]; // 128 chars+ /n + /0
        fgets(input, MAX_INPUT_LENGTH + 1, stdin);
        // too many characters:
        if (input[strlen(input) - 1] != '\n') {
            while (getchar() != '\n') {}
            printf("ERR\n");
            continue;
        }
        if (strncmp(input, "END", 3) == 0) // if the user entered END to exit
        {
            end = 0;
        }
        sem_wait(empty); // waits until empty!=0 means that there is an empty cell that is free to write in him
        sem_wait(mutex); // waits if the other process is in the critical section right now
        //start of the critical section
        strcpy( & (shm_ptr[index * MAX_INPUT_LENGTH]), input); // write the input to the shared memory
        index++;
        if (index == 10) // if we arrived to the end of the shared memory
        {
            index = 0;
        }
        //end of the critical section
        sem_post(mutex); // free the mutex(mutex++)
        sem_post(full); // full++ because there is one more full cell in the shared memory
        if (end == 0) // if the user entered END to exit
        {
            break;
        }
    }
    //semaphore close:
    sem_close(mutex);
    sem_close(full);
    sem_close(empty);
    //shared memory release:
    shmdt(shm_ptr);
    exit(EXIT_SUCCESS);
}
