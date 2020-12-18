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

void signalHandler(int signal)
{
    ofstream myfile;
    myfile.open(process_output);
    string message;
    if (signal == 15)
    {
        message = "P" + to_string(*process_number) + " waiting for a signal";
        myfile << message << endl;
        exit(15);
    }
    else
    {
        message = "P" + to_string(*process_number) + " recieved signal " + to_string(signal);
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

    process_number = (char *)argv[1];
    process_output = (char *)argv[2];
    cout << process_number << " - " << process_output << endl;
    if (*process_number == '1')
    {
        isHeadProcess = true;
    }

    ofstream myfile;
    myfile.open(process_output);
    string message;

    message = "P" + to_string(*process_number) + " waiting for a signal";
    myfile << message << endl;

    while (true)
    {
        sleep(1);
    }

    return 0;
}


