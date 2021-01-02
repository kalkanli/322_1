#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;
int num_of_processes; /**< Number of processes that will be created and monitored*/
string process_output; /**< Output path of the P#*/
string watchdog_output; /**< Output path of the watchdog process.*/
FILE *watchdog_ofile; /**< Pointer to the output file of the watchdog process*/
struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; /**< 0.3 second wait time.*/

/**
 * @brief Kills all the processes after being called. Usually after P1 is terminated.
 * 
 * @param process_ids Pointer to the array that holds the values of pid of processes.
 */
void process_slaughter(int *process_ids) {
    for (int i = 2; i <= num_of_processes; i++) {
        kill(process_ids[i], SIGTERM);
        nanosleep(&delta, &delta);
    }
}

/**
 * @brief Returns the index of searched element in the array.
 * 
 * @param arr Pointer to an array
 * @param element Element that is being searched for 
 * @param length Length of the array
 * @return int index of searched element. if not found returns -1
 */
int index_of(int *arr, int element, int length) {
    for (int i = 1; i <= length; i++) {
        if (arr[i] == element) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Prints output message to the file that is pointed by o_file. 
 * It picks the message according to output_type parameter, as shown below.
 * This method was written for simplicity and plainnes.
 * 
 * output_type
 * 
 *      a==0 -> Watchdog is terminating gracefully
 * 
 *      a==1 -> P# is started and it has a pid of <PID>
 *  
 *      a==2 -> P1 is killed, all processes must be killed\nRestarting all processes
 *  
 *      a==3 -> P# is killed
 *              Restarting P#
 *  
 * @param output_type int value that implies type of the output.
 * @param o_file file path of the file.
 * @param a int value to print if needed. if not needed, value of a doesn't matter.
 * @param b int value to print if needed. if not needed, value of b doesn't matter.
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
        fprintf(watchdog_ofile, "P%d is killed\nRestarting P%d\n", a, b);
        break;
    case 0:
        fprintf(watchdog_ofile, "Watchdog is terminating gracefully");
        break;
    }
    fclose(watchdog_ofile);
}

/**
 * @brief Writes to pipe messages in the format "P<process_number> <pid>"
 * Written in order to gain simplicity and plainnes.
 * 
 * @param pipe int value of the pipe to be written at
 * @param process_number process number of the process
 * @param pid process id of the process
 */
void write_to_pipe(int pipe, int process_number, int pid) {
    string message = "P" + to_string(process_number) + " " + to_string(pid);
    write(pipe, message.c_str(), 30);
}


/**
 * @brief Initializes a process with a given process number 
 * 
 * @param process_number number of the process that will be initialized
 * @param output pointer to output file of process.
 */
void initialize_process(int process_number, const char *output) {
    char process_number_arg[10];
    sprintf(process_number_arg, "%d", process_number);
    execl("./process", process_number_arg, output, NULL);
}

/**
 * @brief Main function.
 *  
 * @param argc argument count
 * @param argv argument vector
 * @return int 0 when it finishes execution successfuly
 */
int main(int argc, char const *argv[]) {
   
    // Retrieves the arguments.
    num_of_processes = stoi(argv[1]); 
    watchdog_output = argv[3]; 
    
    // Creates the array that will hold the pid of child processes.
    int process_ids[num_of_processes + 1]; 

    // If file already exists this two lines will clear its contents and treat it like a new file.
    fopen(argv[3], "w");
    fopen(argv[2], "w");

    // Creates the pipe and writes its own pid to the pipe.
    int unnamedPipe;
    char *myfifo = (char *)"/tmp/myfifo";
    mkfifo(myfifo, 0644);
    unnamedPipe = open(myfifo, O_WRONLY);
    write_to_pipe(unnamedPipe, 0, getpid());
    process_ids[0] = getpid();


    // Creates number of processes that is given in the parameters. 
    // Also indexes their pid in "process_ids" array 
    for (int i = 1; i <= num_of_processes; i++) { 
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
            nanosleep(&delta, &delta);       
        }
    }

    // Waits for all the children processes to be finished, then takes appropriate action.
    int child_pid;
    while ((child_pid = wait(NULL)) > 0) {
        if (child_pid == -1) { // if returned pid from wait() then there is no child left that is running. 
            break;
        }
        
        int n = index_of(process_ids, child_pid, num_of_processes);
        if (n == 1) { // n being equal to 1 implies P1 is terminated. so every process will be killed and restarted.
            process_slaughter(process_ids);
            print_output(2, argv[3], 0, 0);
            
            for (int i = 1; i <= num_of_processes; i++) {
                child_pid = fork();
                if (child_pid == 0) {
                    initialize_process(i, argv[2]);
                }
                else if (child_pid > 0) {
                    process_ids[i] = child_pid;
                    print_output(1, argv[3], i, child_pid);
                    nanosleep(&delta, &delta);
                    write_to_pipe(unnamedPipe, i, child_pid);
                }
            }
           
        } else if (n > 1) { // One process killed only that process will be restarted.
            process_ids[n] = fork();
            if (process_ids[n] == 0) {
                initialize_process(n, argv[2]);
            }
            write_to_pipe(unnamedPipe, n, process_ids[n]);
            print_output(3, argv[3], n, n);
            print_output(1, argv[3], n, process_ids[n]);
        }
    }

    // Final output printed.
    print_output(0, argv[3], 0, 0);

    return 0;
}
