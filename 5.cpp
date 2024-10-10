/*
Name: Abdullah Bin Zubair
Date: 6th October 2024
Class: COMP350-001
Description of Program: 
This program utilizes several threads to sum a number of integers from an input file. 
The first element in the file is the number of elements to follow. Each thread sums a portion of the list, 
and then the sums from all threads are totaled. The result is output to either the standard display or to "output.txt".
*/

#include <iostream>     // For standard input/output
#include <fstream>      // For file input/output
#include <thread>       // For using threads
#include <vector>       // For storing numbers in a vector
#include <mutex>        // For ensuring safe access to global_sum

std::mutex mtx; // To prevent race conditions when threads modify global_sum
int global_sum = 0; // Global variable to hold the total sum of integers

// Function executed by each thread to sum a portion of the array
void sum_portion(const std::vector<int>& numbers, int start, int end) 
{
    int local_sum = 0;  // Variable to store the sum of the current portion
    // Loop through the assigned portion and calculate the sum
    for (int i = start; i < end; ++i) 
    {
        local_sum += numbers[i];  // Add each element to local_sum
    }
    // Lock the mutex to safely update the global sum
    std::lock_guard<std::mutex> lock(mtx);
    global_sum += local_sum;  // Add the local sum to the global sum
}

int main(int argc, char* argv[]) 
{
    // Check if the correct number of arguments (3) is passed: program name, number of threads, and input file
    if (argc != 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads> <input_file>" << std::endl;
        return 1;  // Return an error if the correct arguments are not passed
    }

    int num_threads = std::stoi(argv[1]);  // Convert the argument to the number of threads
    std::string file_name = argv[2];  // Store the input file name

    // Open the input file for reading
    std::ifstream input_file(file_name);
    if (!input_file.is_open()) 
    {
        std::cerr << "Error opening file " << file_name << std::endl;
        return 1;  // Return an error if the file cannot be opened
    }

    int num_elements;
    input_file >> num_elements;  // First element is the number of remaining integers in the file

    // Create a vector to store the integers from the file
    std::vector<int> numbers(num_elements);
    for (int i = 0; i < num_elements; ++i) 
    {
        input_file >> numbers[i];  // Read each number into the vector
    }
    input_file.close();  // Close the input file

    // If there are more threads than numbers, limit the number of threads to the number of elements
    if (num_threads > num_elements) 
    {
        num_threads = num_elements;
    }

    // Calculate how many elements each thread will handle
    int elements_per_thread = num_elements / num_threads;
    int remainder = num_elements % num_threads;  // Handle any remainder by giving some threads an extra element

    std::vector<std::thread> threads;  // Vector to store the threads
    int start = 0;  // Start index for each thread

    // Create threads and assign each one a portion of the array to sum
    for (int i = 0; i < num_threads; ++i) 
    {
        // Determine the end index for the current thread's portion
        int end = start + elements_per_thread + (i < remainder ? 1 : 0);  // Some threads may sum 1 extra element
        threads.push_back(std::thread(sum_portion, std::ref(numbers), start, end));  // Create the thread
        start = end;  // Update the start index for the next thread
    }

    // Wait for all threads to finish
    for (auto& t : threads) 
    {
        t.join();  // Join each thread (i.e., wait for it to complete)
    }

    // Output the result to both the console and the output file
    std::ofstream output_file("output.txt", std::ofstream::trunc);  // Open (or create) output.txt, truncate if it exists
    if (!output_file.is_open()) 
    {
        std::cerr << "Error creating output.txt" << std::endl;
        return 1;  // Return an error if the output file cannot be opened/created
    }

    output_file << "Total sum: " << global_sum << std::endl;  // Write the total sum to output.txt
    std::cout << "Total sum: " << global_sum << std::endl;  // Also print the total sum to the console
    output_file.close();  // Close the output file

    return 0;  // Program completed successfully
}
