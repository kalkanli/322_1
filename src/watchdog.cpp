/*

- writes to the pipe of the processes with their ids P# <pid>
- initialize all processes using fork() with exec() of processes. 
- if a process is terminated, restart that process and write its pid and P#
- if terminated process is the head process, then kill all processes via SIGTERM signal, 
- check if it is terminated. if so reinitialize.


*/
#include <iostream>
#include <string> 
#include <unistd.h>
#include <fstream>
#include <fcntl.h> 
#include <sys/stat.h> 

using namespace std;
int num_of_processes;
char* process_output;
char* watchdog_output;

int main(int argc, char const *argv[])
{   
    num_of_processes = stoi(argv[1]);
    process_output = (char*) argv[2];
    watchdog_output = (char*) argv[3];

    ofstream myfile;
    myfile.open(watchdog_output);
    char output[50];

    int unnamedPipe;
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0644);
    char temp[30];
    string message;
    unnamedPipe = open(myfifo,O_WRONLY);

    temp[30] = 'P0' + getpid(); 
    write(unnamedPipe, temp, 30);

    for(int i=1; i<=num_of_processes; i++) {

        pid_t child = fork();
        if(child == 0) {
            char process_number = char(i);
            int pid = getpid();
            temp[30] = 'P' + i + ' ' + pid;
            write(unnamedPipe, temp, 30);
            output[50] = 'P' + i + ' is started and it has a pid of ' + pid;
            myfile << output << endl;
            execl("./executor.out", (char*) process_number, process_output); // might not be correct to way.
            break;
        } else if(child == -1) {
            perror("fork failed.");
            break;
        }
    }

    /*
        wait for signals. then do propoper operation.
        wait is necessary since we want watchdog to wait for child processes.
    */

    return 0;
}
