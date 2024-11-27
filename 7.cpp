#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <cstdlib>
#include <unistd.h>
#include <fstream>

using namespace std;

// Shared buffer to store produced widgets
queue<int> buffer;

// Buffer size passed as a command line argument
int buffer_size;

// Counter to keep track of the total number of produced items
int item_count = 0;

// Maximum number of items to produce and consume
const int MAX_ITEMS = 10;

// Mutex to lock critical sections and protect shared resources (buffer)
pthread_mutex_t mutex;

// Semaphores to keep track of available slots in the buffer and the number of widgets
sem_t empty_slots;  // Tracks empty slots in the buffer
sem_t full_slots;   // Tracks the number of items in the buffer

// Output file for logging produced and consumed items
ofstream output("output.txt");

/*
 * Producer thread function:
 * - Each producer thread produces a random widget (represented as a random integer)
 * - The producer waits if the buffer is full
 * - The producer adds the widget to the buffer when space is available
 * - The producer stops after producing at least 10 items
 */
void* producer(void* id) {
    int producer_id = *(int*)id;

    while (true) {
        usleep(rand() % 1000000); // Simulate production time with random delay

        // Produce an item (random number between 0 and 99)
        int item = rand() % 100;

        // Wait if no empty slots are available (buffer is full)
        sem_wait(&empty_slots);

        // Lock the buffer to safely add the item (critical section)
        pthread_mutex_lock(&mutex);

        // If 10 items have already been produced, stop production
        if (item_count >= MAX_ITEMS) {
            pthread_mutex_unlock(&mutex);
            sem_post(&empty_slots); // Release empty slot for other producers
            break;
        }

        // Add the produced item to the buffer
        buffer.push(item);
        item_count++;

        // Log the produced item to standard display and to output.txt
        cout << "Producer " << producer_id << " produced " << item << endl;
        output << "Producer " << producer_id << " produced " << item << endl;

        // Unlock the buffer after adding the item
        pthread_mutex_unlock(&mutex);

        // Signal that the buffer has a new full slot (item available for consumption)
        sem_post(&full_slots);
    }

    pthread_exit(0);
}

/*
 * Consumer thread function:
 * - Each consumer thread waits for an item to be available in the buffer
 * - The consumer removes the item from the buffer and processes it
 * - The consumer stops if there are no more items left to consume
 */
void* consumer(void* id) {
    int consumer_id = *(int*)id;

    while (true) {
        usleep(rand() % 1000000); // Simulate processing time with random delay

        // Wait if no items are available to consume (buffer is empty)
        sem_wait(&full_slots);

        // Lock the buffer to safely remove the item (critical section)
        pthread_mutex_lock(&mutex);

        // If there are no more items left to consume, exit
        if (item_count <= 0 && buffer.empty()) {
            pthread_mutex_unlock(&mutex);
            sem_post(&full_slots);  // Release the full slot for other consumers
            break;
        }

        // Consume the item if the buffer is not empty
        if (!buffer.empty()) {
            int item = buffer.front();
            buffer.pop();  // Remove the item from the buffer

            // Log the consumed item to standard display and to output.txt
            cout << "Consumer " << consumer_id << " consumed " << item << endl;
            output << "Consumer " << consumer_id << " consumed " << item << endl;

            item_count--;
        }

        // Unlock the buffer after consuming the item
        pthread_mutex_unlock(&mutex);

        // Signal that the buffer has an empty slot (ready for new production)
        sem_post(&empty_slots);
    }

    pthread_exit(0);
}

/*
 * Main function:
 * - Accepts command line arguments for the number of producers, consumers, and buffer size
 * - Initializes mutex and semaphores
 * - Creates and manages producer and consumer threads
 * - Waits for all threads to complete and ensures clean-up
 */
int main(int argc, char* argv[]) {
    // Ensure the correct number of arguments are provided
    if (argc != 4) {
        cout << "Usage: " << argv[0] << " <number_of_producers> <number_of_consumers> <buffer_size>" << endl;
        return 1;
    }

    // Parse command line arguments
    int num_producers = atoi(argv[1]);
    int num_consumers = atoi(argv[2]);
    buffer_size = atoi(argv[3]);

    // Initialize mutex and semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty_slots, 0, buffer_size);  // Initialize empty_slots with buffer size
    sem_init(&full_slots, 0, 0);  // Initialize full_slots with 0 items

    // Create producer and consumer threads
    pthread_t producers[num_producers], consumers[num_consumers];
    int producer_ids[num_producers], consumer_ids[num_consumers];

    // Create producer threads
    for (int i = 0; i < num_producers; i++) {
        producer_ids[i] = i;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    // Create consumer threads
    for (int i = 0; i < num_consumers; i++) {
        consumer_ids[i] = i;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }

    // Wait for all producer threads to complete
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }

    // Wait for all consumer threads to complete
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }

    // Cleanup resources
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    output.close();

    // Indicate successful completion
    cout << "Program completed successfully!" << endl;

    return 0;
}
