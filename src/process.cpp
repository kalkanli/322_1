#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <csignal>

using namespace std;
const char *process_output; /**< pointer to the process_output file path.*/
int process_number; /**< N for this process. (PN).*/
FILE *file; /**< File that processes writes concurrently.*/

/**
 * @brief prints output according to the given output type as shown below.
 *     
 * output_type:
 * 
 *     a==1 -> P# is waiting for signal
 * 
 *     a==2 -> P# recieved signal <signal>
 * 
 *     a==3 -> P# recieved signal <signal>, terminating gracefully  
 * 
 * @param output_type defines the type of the message that will be printed to the output file.
 * @param o_file pointer to the file path of the output file.
 * @param signal signal that will be printed in the message.
 * 
 */
void print_output(int output_type, const char *o_file, int process_number, int signal)
{
    file = fopen(o_file, "a");
    switch (output_type)
    {
    case 1:
        fprintf(file, "P%d is waiting for signal\n", process_number);
        break;
    case 2:
        fprintf(file, "P%d recieved signal %d\n", process_number, signal);
        break;
    case 3:
        fprintf(file, "P%d recieved signal %d, terminating gracefully\n", process_number, signal);
        break;
    }
    fclose(file);
}

/**
 * @brief Handles the signals that processes recieves as the name implies.
 * Prints the proper message to output file or exits.
 * 
 * @param signal signal that process recieves.
 */
void signalHandler(int signal)
{
    if (signal == 15)
    {
        print_output(3, process_output, process_number, signal);
        exit(15);
    }
    else
    {
        print_output(2, process_output, process_number, signal);
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

    if (argc == 2) 
    {
        process_number = stoi(argv[0]);
        process_output = argv[1];
    }
    else if (argc == 3)
    {
        process_number = stoi(argv[1]);
        process_output = argv[2];
    }
    else
    {
        perror("wrong number of arguments!");
    }

    print_output(1, process_output, process_number, 0);

    while (true)
    {
        sleep(1);
    }

    return 0;
}
