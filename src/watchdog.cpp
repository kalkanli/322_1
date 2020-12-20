#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <map>

using namespace std;
int num_of_processes;
string process_output;
string watchdog_output;
map<int, int> process_ids = {};

int main(int argc, char const *argv[])
{
    num_of_processes = stoi(argv[1]);
    watchdog_output = argv[3];

    ofstream myfile;
    myfile.open(watchdog_output);

    // int unnamedPipe;
    // char * myfifo = (char*) "/tmp/myfifo";
    // mkfifo(myfifo, 0644);
    // unnamedPipe = open(myfifo, O_WRONLY);
    // string message = "P0 " + to_string(getpid()) + "\n";
    // write(unnamedPipe, message.c_str(), 30);
    int parent_process_id = getpid();
    process_ids.insert(pair<int, int>(parent_process_id, 0));

    int i, pid;
    for (i = 1; i <= num_of_processes; i++)
    {
        int child = fork();
        if (child == 0)
        {
            char process_number[10];
            sprintf(process_number, "%d", i);
            pid = getpid();
            //process_ids.insert(make_pair(pid, i));
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
            process_ids.insert(pair<int, int>(child, i));
        }
    }

    map<int, int>::iterator it;
    int child_pid;
    while ((child_pid = wait(NULL)) > 0)
    {
        it = process_ids.find(child_pid);
        int n = it->second;
        if (it->second == 1)
        {
            string message = "P" + to_string(n) + " is killed\nRestarting P" + to_string(n);
        }
        char process_num[10];
        sprintf(process_num, "%d", n);
        string message = "P" + to_string(n) + " is killed\nRestarting P" + to_string(n);
        myfile << message << endl;
        int child1 = fork();
        process_ids.erase(it);
        process_ids.insert(pair<int, int>(child1, n));
        execl("./process", process_num, argv[2], NULL);
    }

    return 0;
}
