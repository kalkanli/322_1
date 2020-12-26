#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <csignal>

using namespace std;
string process_output;
int process_number;
FILE *file;
string message;

void signalHandler(int signal)
{

    if (signal == 15)
    {
        file = fopen("../testcases/process.txt", "a");
        fprintf (file, "P%d recieved signal %d, terminating gracefully\n",process_number, signal);
        fclose(file);
        exit(15);
    }
    else
    {
        file = fopen("../testcases/process.txt", "a");
        fprintf (file, "P%d recieved signal %d\n",process_number, signal);
        fclose(file);
    }
}


int main(int argc, char const *argv[])
{
    signal(SIGHUP, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGILL, signalHandler);
    signal(SIGTRAP, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGXCPU, signalHandler);

    if(argc == 2) {
        process_number = stoi(argv[0]);
        process_output = argv[1];
    } else if(argc == 3) {
        process_number = stoi(argv[1]);
        process_output = argv[2];
    } else {
        perror("wrong number of arguments!");
    }
    
    file = fopen("../testcases/process.txt", "a");
    fprintf (file, "P%d is waiting for signal\n",process_number);
    fclose(file);

    while (true)
    {
        sleep(1);
    }

    return 0;
}


