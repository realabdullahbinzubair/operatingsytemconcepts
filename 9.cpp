/*
Name: Abdullah Bin Zubair
Date: 3rd November 2024
Class: COMP350-001
Description of Program: 
This program simulates the Santa Claus problem with multiple threads, where Santa helps a group of elves and reindeer. 
It ensures that Santa only helps when there are 3 elves or 9 reindeer present. 
Reindeer have priority over elves when both groups are ready for assistance. 
The program runs until at least one complete group of reindeer and one group of elves have been assisted.
*/

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

// Global variables for the number of elves and reindeer
int elves_count = 0;
int reindeer_count = 0;
const int MAX_REINDEER = 9;
const int MAX_ELVES = 3;

// Mutex and condition variables
mutex mtx;
condition_variable santa_cv;
bool santa_available = true; // Indicates if Santa can help
bool helped_reindeer = false;
bool helped_elves = false;

// Santa's function
void Santa() {
    while (!(helped_reindeer && helped_elves)) {
        unique_lock<mutex> lock(mtx);
        santa_cv.wait(lock, [] { return !santa_available; }); // Wait until Santa is needed

        if (reindeer_count == MAX_REINDEER) {
            cout << "Santa is helping the reindeer!" << endl;
            helped_reindeer = true; // Mark that Santa helped reindeer
            reindeer_count = 0; // Reset reindeer count after helping
        } else if (elves_count == MAX_ELVES) {
            cout << "Santa is helping the elves!" << endl;
            helped_elves = true; // Mark that Santa helped elves
            elves_count = 0; // Reset elves count after helping
        }
        
        santa_available = true; // Santa is available again
        lock.unlock();
    }
}

// Reindeer thread function
void Reindeer() {
    while (true) {
        // Simulate reindeer arriving
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        {
            lock_guard<mutex> lock(mtx);
            reindeer_count++;
            cout << "A reindeer has arrived! Total: " << reindeer_count << endl;

            // If we have enough reindeer, signal Santa
            if (reindeer_count == MAX_REINDEER) {
                santa_available = false; // Santa is busy helping
                santa_cv.notify_one(); // Signal Santa
            }
        }
    }
}

// Elf thread function
void Elf() {
    while (true) {
        // Simulate elves arriving
        this_thread::sleep_for(chrono::milliseconds(rand() % 1000));

        {
            lock_guard<mutex> lock(mtx);
            elves_count++;
            cout << "An elf has arrived! Total: " << elves_count << endl;

            // If we have enough elves and no reindeer waiting, signal Santa
            if (elves_count == MAX_ELVES && reindeer_count < MAX_REINDEER) {
                santa_available = false; // Santa is busy helping
                santa_cv.notify_one(); // Signal Santa
            }
        }
    }
}

int main() {
    // Create Santa thread
    thread santa_thread(Santa);

    // Create reindeer threads
    vector<thread> reindeer_threads;
    for (int i = 0; i < MAX_REINDEER; i++) {
        reindeer_threads.push_back(thread(Reindeer));
    }

    // Create elf threads (can be more than 3)
    vector<thread> elf_threads;
    for (int i = 0; i < 10; i++) {
        elf_threads.push_back(thread(Elf));
    }

    // Join threads (this will be a blocking call)
    santa_thread.join();
    for (auto& t : reindeer_threads) {
        t.join();
    }
    for (auto& t : elf_threads) {
        t.join();
    }

    return 0;
}
