#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <csignal>

char *process_output;
char *process_number;
bool isHeadProcess = false;

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
    
    process_number = argv[1];
    process_output = argv[2];
    if (process_number == '1')
    {
        isHeadProcess = true;
    }

    ofstream myfile;
    myfile.open(process_output);
    char output[50];

    output = "P" + process_number + " waiting for a signal"; 
    myfile << output << endl;

    while(true) {
        sleep(1);
    }

    return 0;
}

void signalHandler(int signal) {
    if(signal == 15) {
        exit(15);
    }
    ofstream myfile;
    myfile.open(process_output);
    char output[50];
    output = process_number "received signal " + signal; 
    myfile << output << endl;
}
