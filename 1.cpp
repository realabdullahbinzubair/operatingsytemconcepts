/* Name: Abdullah Bin Zubair
   Date: 8th September 2024
   Class: COMP350-001
   Description of Program: This C++ program creates a child process using the fork() function. It shows messages from both the parent and 
   child processes, along with their PIDs. If something goes wrong with fork(), it prints an error message and exits cleanly. The parent 
   process also displays the PID of the child process.
*/

#include <iostream>
#include <unistd.h> // for fork(), getpid(), and getppid()
#include <sys/types.h> // for pid_t
#include <cstdlib> // for exit()
using namespace std;

int main() {
    pid_t pid = fork(); // fork a new process

    if (pid < 0)
    {
        // If fork() returns a negative value, an error occurred
        cerr << "Fork failed, unable to create a child process." << endl;
        exit(1); // Exit the program with a failure code
    }

    else if (pid == 0)
    {
        // This block will execute in the child process
        cout << "This is the child process. PID: " << getpid() << endl;
        //cout << "Parent PID (from child): " << getppid() << endl;
    }
    
    else
    {
        // This block will execute in the parent process
        cout << "This is the parent process. PID: " << getpid() << endl;
        cout << "Child process ID: " << pid << endl;
    }

    return 0; // Exit successfully
}


