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

using namespace std;

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
    
    process_number = (char*) argv[1];
    process_output = (char*) argv[2];
    if ((char)process_number == '1')
    {
        isHeadProcess = true;
    }

    ofstream myfile;
    myfile.open(process_output);
    char output[50];

    output[50] = 'P' + (char)process_number + ' waiting for a signal'; 
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
    output[50] = 'P' + (char)process_number + ' waiting for a signal'; 
    myfile << output << endl;
}
