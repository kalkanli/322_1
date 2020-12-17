/*

- writes to the pipe of the processes with their ids P# <pid>
- initialize all processes using fork() with exec() of processes. 
- if a process is terminated, restart that process and write its pid and P#
- if terminated process is the head process, then kill all processes via SIGTERM signal, 
- check if it is terminated. if so reinitialize.


*/
#include <string> 
#include <unistd.h>

using namespace std;
int num_of_processes;
char* process_output;
string watchdog_output;

int main(int argc, char const *argv[])
{   
    num_of_processes = stoi(argv[1]);
    process_output = (char*) argv[2];
    watchdog_output = argv[3];

    for(int i=0; i<num_of_processes; i++){
        pid_t child = fork();
        if(child == 0) {
            char process_number = char(i);
            execl(process_output, (char*)process_number);
        }
    }
    
    return 0;
}
