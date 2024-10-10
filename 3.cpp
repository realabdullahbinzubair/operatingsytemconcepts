/* 
Name: Abdullah Bin Zubair
Date: 22nd September 2024
Class: COMP350-001
Description of Program: 
This program demonstrates inter-process communication using a pipe. 
It forks a child process that writes a message ("Hello World") to a pipe, 
which the parent process reads and outputs to either a file, 
the terminal, or both, based on command-line arguments. 
*/

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>
using namespace std;

// Function to print the usage instructions
void print_usage() {
    cout << "Usage: ./3 [-o] [-b]" << endl;
    cout << " -o : Redirect output to output.txt only." << endl;
    cout << " -b : Redirect output to output.txt and the terminal." << endl;
}

int main(int argc, char* argv[])
{
    int pipe_fd[2]; // Array to hold the file descriptors for the pipe
    pid_t pid; // Variable to hold the process ID

    // Create a pipe
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe"); // Print error if pipe creation fails
        return 1;
    }

    // Fork the process to create a child
    pid = fork();
    if (pid < 0)
    {
        perror("fork"); // Print error if fork fails
        return 1;
    }

    if (pid == 0)
    { // Child process block
        close(pipe_fd[0]); // Close the unused read end of the pipe

        // Message to write to the pipe
        const char* msg = "Hello World\n";
        write(pipe_fd[1], msg, strlen(msg)); // Write the message to the pipe
        close(pipe_fd[1]); // Close the write end of the pipe
        return 0; // Exit child process
    } 
    else
    { // Parent process block
        close(pipe_fd[1]); // Close the unused write end of the pipe

        // Initialize flags for command-line arguments
        bool output_to_file = false;
        bool output_to_both = false;

        // Process command-line arguments
        for (int i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "-o") == 0)
            {
                output_to_file = true; // Set flag for output to file
            } else if (strcmp(argv[i], "-b") == 0)
            {
                output_to_both = true; // Set flag for output to both
                output_to_file = true; // -b implies -o
            } else
            {
                print_usage(); // Print usage if unknown argument
                return 1; // Exit with error
            }
        }

        // Create or truncate output.txt if needed
        int output_fd = -1;
        if (output_to_file)
        {
            output_fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open file for writing
            if (output_fd < 0)
            {
                perror("open"); // Print error if file opening fails
                return 1;
            }
        }

        // Redirect the output based on flags
        if (output_to_both)
        {
            // Duplicate the file descriptor to stdout
            dup2(output_fd, STDOUT_FILENO); // Redirect to file
            cout << "Redirected output to both terminal and file." << endl; // Notify user
            dup2(output_fd, STDOUT_FILENO); // Redirect to file again
        } 
        else if (output_to_file)
        {
            dup2(output_fd, STDOUT_FILENO); // Redirect to file only
        }

        // Read from the pipe and print to stdout (or redirected output)
        char buffer[256]; // Buffer to hold data read from the pipe
        ssize_t bytes_read; // Variable to track number of bytes read
        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0)
        {
            write(STDOUT_FILENO, buffer, bytes_read); // Write buffer contents to output
        }
        
        // Clean up resources
        close(pipe_fd[0]); // Close read end of the pipe
        if (output_fd >= 0)
        {
            close(output_fd); // Close output file descriptor if opened
        }
        wait(NULL); // Wait for the child process to finish
    }

    return 0; // Exit the parent process
}
