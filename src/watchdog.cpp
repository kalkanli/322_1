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

/*
    output_type
    1 -> P# is started and it has a pid of <PID>.
    2 -> P1 is killed, all processes ...
    3 -> P# is killed
         Restarting P#
*/
void print_output(int output_type, const char *o_file, int a, int b) {
    watchdog_ofile = fopen(o_file, "a");
    switch (output_type) {
    case 1:
        fprintf(watchdog_ofile, "P%d is started and it has a pid of %d\n", a, b);
        break;
    case 2:
        fprintf(watchdog_ofile, "P1 is killed, all processes must be killed\nRestarting all processes\n");
        break;
    case 3:
        fprintf(watchdog_ofile, "P%d is is killed\nRestarting P%d\n", a, b);
        break;
    }
    fclose(watchdog_ofile);
}

void write_to_pipe(int pipe, int process_number, int pid) {
    string message = "P" + to_string(process_number) + " " + to_string(pid);
    write(pipe, message.c_str(), 30);
}

void initialize_process(int process_number, const char *output) {
    char process_number_arg[10];
    sprintf(process_number_arg, "%d", process_number);
    execl("./process", process_number_arg, output, NULL);
}

int main(int argc, char const *argv[]) {

    num_of_processes = stoi(argv[1]);
    watchdog_output = argv[3];
    int process_ids[num_of_processes + 1];

    int unnamedPipe;
    char *myfifo = (char *)"/tmp/myfifo";
    mkfifo(myfifo, 0644);
    unnamedPipe = open(myfifo, O_WRONLY);

    write_to_pipe(unnamedPipe, 0, getpid());

    int parent_process_id = getpid();
    process_ids[0] = parent_process_id;
    int i;
    for (i = 1; i <= num_of_processes; i++) {
        int child = fork();
        if (child == 0) {

            write_to_pipe(unnamedPipe, i, getpid());
            print_output(1, argv[3], i, getpid());
            initialize_process(i, argv[2]);
        
        } else if (child == -1) {
            perror("fork failed.");
            break;
        } else {
            process_ids[i] = child;
        }
    }

    int child_pid;
    while ((child_pid = wait(NULL)) > 0) {
        if (child_pid == -1) {
            break;
        }
        int n = index_of(process_ids, child_pid, num_of_processes);
        if (n == 1) {
            process_slaughter(process_ids);
            print_output(2, argv[3], 0, 0);
            for (int i = 1; i <= num_of_processes; i++) {
                child_pid = fork();
                if (child_pid == 0) {
                    initialize_process(i, argv[2]);
                }
                else if (child_pid > 0) {
                    process_ids[i] = child_pid;
                    write_to_pipe(unnamedPipe, i, child_pid);
                }
            }
        } else if (n > 1) {
            
            process_ids[n] = fork();
            if (process_ids[n] == 0) {
                initialize_process(n, argv[2]);
            }
            write_to_pipe(unnamedPipe, n, process_ids[n]);
            print_output(3, argv[3], n, n);
        }
    }
    fclose(watchdog_ofile);

    return 0;
}
