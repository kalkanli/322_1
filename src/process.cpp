#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

char *process_output;
char *process_number;
bool isHeadProcess = false;

int main(int argc, char const *argv[])
{
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

    return 0;
}
