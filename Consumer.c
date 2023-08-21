#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <semaphore.h>

#include <pthread.h>

#define MAX_INPUT_LENGTH 129
#define MAX_BUFFER_LENGTH 1291
// a global struct that includes all the exercise information and the result also
typedef struct {
    int firstSize;
    int secondSize;
    int * firstCoefficients;
    int * secondCoefficients;
    char * func;
    int * result;
}exercise;
int resSize = 0;
exercise inputEx;
void poly_position(char * input);
int poly_init(int ** , char * );
int * multiply();
void printPoly();
void polyFunc();
void * threads_func(void * );
int main() {
    sem_t * mutex, * empty, * full;
    mutex = sem_open("/my_mutex1", 0);
    if (mutex == SEM_FAILED) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    full = sem_open("/my_mutex2", 0);
    if (full == SEM_FAILED) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }

    empty = sem_open("/my_mutex3", 0);
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
    int shm_id;
    shm_id = shmget(key, 0, 0666);
    if (shm_id == -1) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    char * shm_ptr;
    shm_ptr = (char * ) shmat(shm_id, NULL, 0);
    if (shm_ptr == (char * ) - 1) {
        perror("ERR");
        exit(EXIT_FAILURE);
    }
    char * savedPtr1;
    int index = 0;
    while (1) {
        sem_wait(full); //blocks the process from reading if there is no full cells to read
        sem_wait(mutex); //blocks the process form entering to his critical section if the writer is in his
        char * shm_input = (char * ) malloc(sizeof(char) * MAX_BUFFER_LENGTH);
        char * temp = (char * )malloc(sizeof(char) * MAX_BUFFER_LENGTH);
        // start of critical section
        strcpy(temp, & shm_ptr[index * MAX_INPUT_LENGTH]); //cope the shared memory to temp
        index++;
        if (index == 10) // if we arrived to the end of the shared memory
        {
            index = 0;
        }
        strcpy(shm_input, strtok(temp, "\n")); //taking one exercise from the shared memory
        if (strncmp(shm_input, "END", 3) == 0) // if the user want to exit:
        {
            free(shm_input);
            free(temp);
            sem_close(mutex);
            sem_close(full);
            sem_close(empty);
            shmdt(shm_ptr);
            if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
                perror("ERR");
                exit(EXIT_FAILURE);
            }
            sem_unlink("/my_mutex1");
            sem_unlink("/my_mutex2");
            sem_unlink("/my_mutex3");
            exit(EXIT_SUCCESS);
        }
        // end of critical section
        sem_post(mutex); // release the mutex by mutex++
        sem_post(empty); // increase the empty cell to one more that can be writen in it
        char * oneEx;
        oneEx = strtok_r(shm_input, "\n", & savedPtr1);
        poly_position(oneEx);
        polyFunc();
        free(shm_input);
        free(temp);
    }
}
// a function that convert the string to an array of ints
void poly_position(char * input) {
    char * temp1 = input + 1, * curr, * endPtr, * savedPtr;
    curr = strtok_r(temp1, ":", & savedPtr);
    if (curr == NULL) {
        return;
    }
    int firstRank = (int) strtol(curr, & endPtr, 10);
    inputEx.firstCoefficients = (int * ) malloc(sizeof(int) * (firstRank + 1));
    if (inputEx.firstCoefficients == NULL) {
        printf("ERR\n");
        exit(EXIT_FAILURE);
    }
    curr = strtok_r(NULL, ")", & savedPtr); // reads the degree of the first polynomial
    if (curr == NULL) {
        printf("ERR\n");
        free(inputEx.firstCoefficients);
        return;
    }
    inputEx.firstSize = poly_init( & inputEx.firstCoefficients, curr); // reads the first polynomial
    inputEx.func = strtok_r(NULL, "(", & savedPtr); // reads the function to do
    if (inputEx.func == NULL) {
        printf("ERR\n");
        free(inputEx.firstCoefficients);
        return;
    }
    curr = strtok_r(NULL, ":", & savedPtr); // reads the degree of the second polynomial
    if (curr == NULL) {
        printf("ERR\n");
        free(inputEx.firstCoefficients);
        return;
    }
    int secondRank = (int) strtol(curr, & endPtr, 10); //turns the degree to an int
    inputEx.secondCoefficients = (int * ) malloc(sizeof(int) * (secondRank + 1));
    if (inputEx.secondCoefficients == NULL) {
        printf("ERR\n");
        free(inputEx.firstCoefficients);
        return;
    }
    curr = strtok_r(NULL, ")", & savedPtr); // reads the second polynomial
    if (curr == NULL) {
        printf("ERR\n");
        free(inputEx.firstCoefficients);
        free(inputEx.secondCoefficients);
        return;
    }
    inputEx.secondSize = poly_init( & inputEx.secondCoefficients, curr); // turns the second polynomial to ints
}
// a function that gets a polynomial type string and initialize the array to his values.
int poly_init(int ** poly, char * src) {
    char * end;
    int index = 0;
    char * number = strtok(src, ",");
    while (number != NULL) {
        ( * poly)[index] = (int) strtol(number, & end, 10); // converts the chars to ints
        index++;
        number = strtok(NULL, ",");
    }
    return index;
}
// a function the doing multiply on two functions
int * multiply() {
    resSize = (inputEx.firstSize + inputEx.secondSize) - 1;
    int * result = (int * ) malloc(sizeof(int) * resSize);
    for (int i = 0; i < resSize; i++) {
        result[i] = 0;
    }
    for (int i = 0; i < inputEx.firstSize; i++) {
        for (int j = 0; j < inputEx.secondSize; j++) {
            result[i + j] += inputEx.firstCoefficients[i] * inputEx.secondCoefficients[j];
        }
    }
    return result;
}
// a function that prints the polynomial values
void printPoly() {
    int isZero = 1;
    int degree = resSize - 1;
    int isFirstNonZero = 1; // Flag to track the first non-zero coefficient

    for (int i = 0; i < resSize; i++) {
        if (inputEx.result[i] != 0) {
            // Print the sign if it's not the first non-zero coefficient
            if (!isFirstNonZero) {
                if (inputEx.result[i] > 0)
                    printf(" + ");
                else
                    printf(" - "); // when the value is negative
            } else {
                // Reset the flag and print the negative sign if necessary
                isFirstNonZero = 0;
                if (inputEx.result[i] < 0)
                    printf("-");
            }

            // Print the coefficient and exponent
            if (degree - i == 0) {
                printf("%d", abs(inputEx.result[i])); // prints the absolute value of the coefficient
            } else if (degree - i == 1) {
                printf("%dx", abs(inputEx.result[i])); // prints the absolute value of the coefficient
            } else {
                printf("%dx^%d", abs(inputEx.result[i]), degree - i); // prints the absolute value of the coefficient
            }

            isZero = 0;
        }
    }

    if (isZero) // if the total result is 0.
    {
        printf("%d", isZero - 1);
    }

    printf("\n");
}
void polyFunc() {
    if (strcmp(inputEx.func, "MUL") == 0) {
        inputEx.result = ( & multiply)();
    } else {
        int size1 = inputEx.firstSize;
        int size2 = inputEx.secondSize;
        int maxSize = size1 > size2 ? size1 : size2;
        resSize = maxSize;
        inputEx.result = (int * ) calloc(resSize, sizeof(int));
        pthread_t threads[resSize]; //creating an array of threads
        int arr[resSize]; // an array for the indexes to calculate
        for (int i = 0; i < resSize; i++) {
            arr[i] = i;
            int x = pthread_create( & threads[i], NULL, threads_func, (void * ) & arr[i]);
            if (x != 0) {
                perror("ERR");
                exit(EXIT_FAILURE);
            }
            // creating resSize pthreads that each one is calculating the-I index in the result
        }
        for (int j = 0; j < resSize; j++) {
            int y = pthread_join(threads[j], NULL); //waits of the threads
            if (y != 0) {
                perror("ERR");
                exit(EXIT_FAILURE);
            }
        }
    }
    printPoly(); //prints the result
    free(inputEx.result);
    free(inputEx.firstCoefficients);
    free(inputEx.secondCoefficients);
    inputEx.result = NULL;
    inputEx.firstCoefficients = NULL;
    inputEx.secondCoefficients = NULL;
}
void * threads_func(void * index) {
    int size1, size2;
    int resDegree = resSize - 1;
    size1 = inputEx.firstSize - 1;
    size2 = inputEx.secondSize - 1;
    int i = * (int * ) index;
    if (size1 >= i) {
        inputEx.result[resDegree - i] += inputEx.firstCoefficients[size1 - i];
    }
    if (size2 >= i) {
        if (strcmp(inputEx.func, "ADD") == 0) {
            inputEx.result[resDegree - i] += inputEx.secondCoefficients[size2 - i]; // doing ADD
        } else inputEx.result[resDegree - i] -= inputEx.secondCoefficients[size2 - i]; // doing SUB
    }
    pthread_exit(NULL); //exiting the thread,and only him(not the others that have the same PCB)
}
