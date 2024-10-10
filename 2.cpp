/* 
Name: Abdullah Bin Zubair
Date: 17th September 2024
Class: COMP350-001
Description of Program: This C++ program creates a child process using the fork() function and runs the 'wc' command on the '1.cpp' file
using execvp(). It ensures proper error handling for the fork() and execvp() functions and prints the word count results from the child process.
*/

#include <iostream>
#include <unistd.h>  // for fork(), execvp(), getpid()
#include <sys/types.h>  // for pid_t
#include <sys/wait.h>   // for wait()
#include <cstdlib>      // for exit()

using namespace std;

int main() {
    pid_t pid = fork(); // fork a new process

    if (pid < 0) {
        // If fork() returns a negative value, an error occurred
        cerr << "Fork failed, unable to create a child process." << endl;
        exit(1); // Exit the program with a failure code
    }

    else if (pid == 0) {
        // This block will execute in the child process
        cout << "This is the child process. PID: " << getpid() << endl;

        // Prepare the arguments for execvp
        char *args[] = {(char *)"wc", (char *)"1.cpp", NULL};

        // Execute the wc command on 1.cpp using execvp()
        if (execvp(args[0], args) == -1) {
            // If execvp fails, print an error message and exit
            cerr << "Error executing wc on 1.cpp" << endl;
            exit(1);
        }
    }

    else {
        // This block will execute in the parent process
        cout << "This is the parent process. PID: " << getpid() << endl;
        cout << "Child process ID: " << pid << endl;

        // Wait for the child process to finish
        wait(NULL);
        cout << "Child process completed." << endl;
    }

    return 0; // Exit successfully
}

