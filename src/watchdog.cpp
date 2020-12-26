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
FILE *watchdog_ofile;

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

int main(int argc, char const *argv[]) {
    num_of_processes = stoi(argv[1]);
    watchdog_output = argv[3];
    int process_ids[num_of_processes + 1];
    int unnamedPipe;
    char * myfifo = (char*) "/tmp/myfifo";
    mkfifo(myfifo, 0644);
    unnamedPipe = open(myfifo, O_WRONLY);
    string message = "P0 " + to_string(getpid()) + "\n";
    write(unnamedPipe, message.c_str(), 30);
    int parent_process_id = getpid();
    process_ids[0] = parent_process_id;
    int i, pid;
    for (i = 1; i <= num_of_processes; i++) {
        int child = fork();
        if (child == 0) {
            char process_number[10];
            sprintf(process_number, "%d", i);
            pid = getpid();
            message = "P" + to_string(i) + " " + to_string(pid) + "\n";
            write(unnamedPipe, message.c_str(), 30);
            watchdog_ofile = fopen("../testcases/watchdog.txt", "a");
            fprintf(watchdog_ofile, "P%d is started and it has a pid of %d\n", i, pid);
            fclose(watchdog_ofile);
            execl("./process", process_number, argv[2], NULL);
        } else if (child == -1) {
            perror("fork failed.");
            break;
        } else {
            process_ids[i] = child;
        }
    }

    int child_pid;
    int status;
    while ((child_pid = wait(NULL)) > 0) {
        if(child_pid == -1) { break; }
        int n = index_of(process_ids, child_pid, num_of_processes);
        if (n == 1) {
            process_slaughter(process_ids);
            watchdog_ofile = fopen("../testcases/watchdog.txt", "a");
            fprintf(watchdog_ofile, "P1 is killed, all processes must be killed\nRestarting all processes\n");
            fclose(watchdog_ofile);
            for(int i=1; i<=num_of_processes; i++) {
                child_pid = fork();
                if(child_pid == 0) {
                    char process_number[10];
                    sprintf(process_number, "%d", i);
                    execl("./process", process_number, argv[2], NULL);
                } else if(child_pid > 0) {
                    process_ids[i] = child_pid;
                    message = "P" + to_string(i) + " " + to_string(child_pid) + "\n";
                    write(unnamedPipe, message.c_str(), 30);
                }
            }
        
        } else if (n > 1) {
            char process_number[10];
            process_ids[n] = fork();
            if(process_ids[n] == 0) {
                sprintf(process_number, "%d", n);
                execl("./process", process_number, argv[2], NULL);
            }
            message = "P" + to_string(n) + " " + to_string(process_ids[n]) + "\n";
            write(unnamedPipe, message.c_str(), 30);
            watchdog_ofile = fopen("../testcases/watchdog.txt", "a");
            fprintf(watchdog_ofile, "P%d is is killed\nRestarting P%d\n", n, n);
            fclose(watchdog_ofile);
        }
    }
    fclose(watchdog_ofile);

    return 0;
}
