#include <iostream>
#include <string> 
#include <unistd.h>
#include <fstream>
#include <fcntl.h> 
#include <sys/stat.h> 
#include<sys/wait.h> 



using namespace std;
int num_of_processes;
string process_output;
string watchdog_output;

int main(int argc, char const *argv[])
{   
    cout << argv[1] << endl;
    num_of_processes = stoi(argv[1]);
    process_output = argv[2];
    watchdog_output = argv[3];

    ofstream myfile;
    myfile.open(watchdog_output);

    int unnamedPipe;
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0644);
    string message = "P0 " + to_string(getpid()); 


    write(unnamedPipe, message.c_str(), 30);
    int i, pid;
    
    for(i=1; i<=num_of_processes; i++) {

        pid_t child = fork();
        if(child == 0) {
            const char number = char(i);
            pid = getpid();
            message = "P" + to_string(i) + " " + to_string(pid);
            write(unnamedPipe, message.c_str(), 30);
            string output = "P" + to_string(i) + " is started and it has a pid of " + to_string(pid);
            myfile << output << endl;
            execl("/mnt/c/Users/ABDULLAH/Desktop/322_1/src/process", &number, process_output, NULL); // might not be correct to way.
            break;
        } else if(child == -1) {
            perror("fork failed.");
            break;
        }
    }

    while(1) {
        sleep(1);
    }

    while(wait(NULL) > 0) {
        if(i == 1) {
            cout << "kill' em all " << i << endl;
            // kill all processes. reinitialize all processes.
        } else {
            cout << "time to reinitialize " << i << endl;
        }
    }
    
    /*
        wait for signals. then do propoper operation.
        wait is necessary since we want watchdog to wait for child processes.
    */

    return 0;
}
