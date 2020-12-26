#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;
int num_of_processes;
string process_output;
string watchdog_output;

void process_slaughter(int *process_ids) {
    for (int i = 2; i <= num_of_processes; i++) {
        kill(process_ids[i], SIGTERM);
    }
}

int index_of(int *arr, int element, int length) {
    for (int i = 1; i <= length; i++) {
        if (arr[i] == element) {
            return i;
        }
    }
    return -1;
}

int main(int argc, char const *argv[])
{
    num_of_processes = stoi(argv[1]);
    watchdog_output = argv[3];
    int process_ids[num_of_processes + 1];
    ofstream myfile;
    myfile.open(watchdog_output);

    // int unnamedPipe;
    // char * myfifo = (char*) "/tmp/myfifo";
    // mkfifo(myfifo, 0644);
    // unnamedPipe = open(myfifo, O_WRONLY);
    // string message = "P0 " + to_string(getpid()) + "\n";
    // write(unnamedPipe, message.c_str(), 30);
    int parent_process_id = getpid();
    process_ids[0] = parent_process_id;

    int i, pid;
    for (i = 1; i <= num_of_processes; i++)
    {
        int child = fork();
        if (child == 0)
        {
            char process_number[10];
            sprintf(process_number, "%d", i);
            pid = getpid();
            //message = "P" + to_string(i) + " " + to_string(pid) + "\n";
            //write(unnamedPipe, message.c_str(), 30);
            string output = "P" + to_string(i) + " is started and it has a pid of " + to_string(pid);
            myfile << output << endl;
            execl("./process", process_number, argv[2], NULL);
        }
        else if (child == -1)
        {
            perror("fork failed.");
            break;
        }
        else
        {
            process_ids[i] = child;
        }
    }

    int child_pid;
    while ((child_pid = wait(NULL)) > 0)
    {
        int n = index_of(process_ids, child_pid, num_of_processes);
        if (n == 1)
        {
            process_slaughter(process_ids);
        }
        else
        {
            char process_number[10];
            process_ids[n] = fork();
            sprintf(process_number, "%d", n);
            cout << "P" + to_string(n) + " is killed\nRestarting P"+to_string(n);
            execl("./process", process_number, argv[2], NULL);
        }
    }

    return 0;
}
