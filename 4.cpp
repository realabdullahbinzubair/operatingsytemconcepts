/* 
Name: Abdullah Bin Zubair
Date: 29th September 2024
Class: COMP350-001
Description of Program: 
This program demonstrates the creation of two threads using the pthread library in C++. 
Each thread will count from 0 to 10, and output each iteration of the count using `std::cout`. 
The main function creates two threads, and then waits for both threads to finish before exiting.
*/

#include <iostream>
#include <pthread.h>
#include <unistd.h> // For usleep

// Function for counting from 0 to 10 in each thread
void* count(void* thread_id) {
    long tid = (long)thread_id;  // Cast thread_id to long type for thread number
    for (int i = 0; i <= 10; i++) {
        std::cout << "Thread " << tid << " counting: " << i << std::endl;
        usleep(100000); // Sleep to make output more readable by adding delay between iterations
    }
    pthread_exit(NULL); // Exit the thread safely
}

int main() {
    pthread_t threads[2];  // Array to hold two thread objects

    // Creating two threads
    for (long i = 0; i < 2; i++) {
        int rc = pthread_create(&threads[i], NULL, count, (void*)i);  // Create threads and pass their ID
        if (rc) {
            std::cout << "Error: unable to create thread, " << rc << std::endl;
            exit(-1);  // Exit if thread creation fails
        }
    }

    // Joining threads to ensure main waits for their completion
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);  // Wait for each thread to finish
    }

    pthread_exit(NULL);  // Exit main thread
}
// g++ -pthread 4.cpp -o thread_count
