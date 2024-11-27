/*
Name: Abdullah Bin Zubair
Date: 15th October 2024
Class: COMP350-001
Description of Program: 
This program simulates the classic Producer-Consumer problem using multiple threads. The program accepts two command-line arguments: 
the number of producer threads and the number of consumer threads. Producers generate widgets and place them into an infinite buffer, 
while consumers retrieve and consume those widgets from the buffer. To ensure synchronization and avoid issues like race conditions, the
program employs semaphores and mutex locks to control access to the buffer. The goal of the program is to produce and consume at least 10 
widgets per producer and consumer without deadlock or crashes. The output is displayed in the console and saved to a file, output.txt.
*/


#include <iostream>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <unistd.h>
#include <fstream>

using namespace std;

// Shared resources
queue<int> buffer; // Infinite buffer for the purposes of this assignment
sem_t full; // Semaphore for full slots
pthread_mutex_t mutex; // Mutex lock for accessing the buffer

ofstream output_file("output.txt");

// Function for producers to produce widgets
void* producer(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < 10; i++) { // Each producer produces 10 widgets
        pthread_mutex_lock(&mutex); // Lock the buffer access

        buffer.push(i); // Produce widget
        string message = "Producer " + to_string(id) + " produced widget " + to_string(i) + "\n";
        cout << message;
        output_file << message;

        pthread_mutex_unlock(&mutex); // Unlock the buffer
        sem_post(&full); // Signal that a widget is available
        sleep(1); // Simulate time taken to produce a widget
    }
    return NULL;
}

// Function for consumers to consume widgets
void* consumer(void* arg) {
    int id = *((int*)arg);
    for (int i = 0; i < 10; i++) { // Each consumer consumes 10 widgets
        sem_wait(&full); // Wait if buffer is empty
        pthread_mutex_lock(&mutex); // Lock the buffer access

        int widget = buffer.front();
        buffer.pop(); // Consume widget
        string message = "Consumer " + to_string(id) + " consumed widget " + to_string(widget) + "\n";
        cout << message;
        output_file << message;

        pthread_mutex_unlock(&mutex); // Unlock the buffer
        sleep(1); // Simulate time taken to consume a widget
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <number_of_producers> <number_of_consumers>" << endl;
        return 1;
    }

    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);

    // Initialize semaphores
    sem_init(&full, 0, 0); // Start with no widgets in the buffer
    pthread_mutex_init(&mutex, NULL);

    // Create producer and consumer threads
    pthread_t producers[num_producers], consumers[num_consumers];
    int producer_ids[num_producers], consumer_ids[num_consumers];

    // Create producer threads
    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for all producers and consumers to finish
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Destroy semaphores and mutex
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    output_file.close(); // Close the output file

    return 0;
}
