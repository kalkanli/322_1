#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <csignal>

using namespace std;
string process_output;
string process_number;
bool isHeadProcess = false;

void signalHandler(int signal)
{
    ofstream myfile;
    myfile.open(process_output);
    string message;
    if (signal == 15)
    {
        message = "P" + process_number + " is killed";
        if(process_number == "1") {
            message += ", all processes must be killed\nRestarting all processes";
        }
        myfile << message << endl;
        exit(15);
    }
    else
    {
        message = "P" + process_number + " recieved signal " + to_string(signal);
        myfile << message << endl;
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

    if(argc != 3) {
        exit(21);
    }
    
    process_number = argv[0];
    process_output = argv[1];

    ofstream myfile;
    myfile.open(process_output);
    string message;

    cout << "process #" + process_number << " is initialized " << endl;
    myfile << process_number << " **** " << process_output << endl;

    message = "P" + process_number + " waiting for a signal";
    myfile << message << endl;

    while (true)
    {
        sleep(1);
    }

    return 0;
}


