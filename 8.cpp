/*
Name: Abdullah Bin Zubair
Date: 27th October 2024
Class: COMP350-001
Description of Program: 
This program simulates the classic Dining Philosophers problem using multiple threads. 
The program accepts two command-line arguments: the number of philosophers and the number of utensils. 
Each philosopher alternates between thinking and eating, while sharing a limited number of utensils to avoid deadlock and starvation. 
The goal is to ensure that each philosopher eats at least 5 times without deadlock or starvation. 
Output is displayed in the console.
*/

#include <iostream>                // For console input/output
#include <thread>                  // For thread management
#include <mutex>                   // For mutex locks
#include <vector>                  // For using vectors
#include <chrono>                  // For sleep duration
#include <condition_variable>       // For condition variables

class DiningPhilosophers {
public:
    // Constructor to initialize the number of philosophers and utensils
    DiningPhilosophers(int num_philosophers, int num_utensils)
        : philosophers(num_philosophers), utensils(num_utensils, true), eat_count(num_philosophers, 0) {}

    // Function to start the philosopher threads
    void start() {
        std::vector<std::thread> threads;  // Vector to hold philosopher threads
        for (int i = 0; i < philosophers; ++i) {
            threads.push_back(std::thread(&DiningPhilosophers::philosopher, this, i)); // Create a philosopher thread
        }

        // Join all threads to the main thread
        for (auto& thread : threads) {
            thread.join();
        }
    }

private:
    int philosophers;                 // Number of philosophers
    std::vector<bool> utensils;      // Availability of utensils; true if available
    std::vector<int> eat_count;      // Count of how many times each philosopher has eaten
    std::mutex mtx;                  // Mutex for synchronization
    std::condition_variable cv;       // Condition variable for signaling between threads

    // Function representing the life of a philosopher
    void philosopher(int id) {
        while (eat_count[id] < 5) {  // Ensure each philosopher eats at least 5 times
            think(id);                // Philosopher thinks
            if (get_utensils(id)) {  // Try to get utensils
                eat(id);             // Philosopher eats
                release_utensils(id); // Release the utensils after eating
            }
        }
    }

    // Function to simulate thinking
    void think(int id) {
        std::cout << "Philosopher " << id << " is thinking." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000)); // Random sleep to simulate thinking time
    }

    // Function to acquire utensils
    bool get_utensils(int id) {
        std::unique_lock<std::mutex> lock(mtx); // Acquire lock for thread safety
        int left = id;                           // Index of the left utensil
        int right = (id + 1) % philosophers;    // Index of the right utensil

        // Wait until both utensils are available
        while (!utensils[left] || !utensils[right]) {
            cv.wait(lock);  // Wait on the condition variable if utensils are not available
        }

        // Acquire both utensils
        utensils[left] = false;  // Take the left utensil
        utensils[right] = false; // Take the right utensil
        return true;
    }

    // Function to simulate eating
    void eat(int id) {
        std::cout << "Philosopher " << id << " is eating." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000)); // Random sleep to simulate eating time
        eat_count[id]++;  // Increment the eating count for this philosopher
    }

    // Function to release utensils after eating
    void release_utensils(int id) {
        std::unique_lock<std::mutex> lock(mtx); // Acquire lock for thread safety
        int left = id;                           // Index of the left utensil
        int right = (id + 1) % philosophers;    // Index of the right utensil

        // Release both utensils
        utensils[left] = true;  // Release the left utensil
        utensils[right] = true; // Release the right utensil
        cv.notify_all();        // Notify all waiting philosophers that utensils are available
    }
};

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_philosophers> <number_of_utensils>" << std::endl;
        return 1;
    }

    // Convert arguments to integers
    int num_philosophers = std::stoi(argv[1]);
    int num_utensils = std::stoi(argv[2]);

    // Validate input values
    if (num_philosophers < 4 || num_utensils < 4) {
        std::cerr << "Number of philosophers and utensils must be at least 4." << std::endl;
        return 1;
    }

    // Create and start the Dining Philosophers simulation
    DiningPhilosophers dp(num_philosophers, num_utensils);
    dp.start();

    return 0; // Indicate successful termination
}
