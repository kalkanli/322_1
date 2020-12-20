#include <iostream>
#include <string> 
#include <unistd.h>
#include <fstream>
#include <fcntl.h> 
#include <sys/stat.h> 
#include<sys/wait.h> 

using namespace std;

int main(int argc, char const *argv[])
{
    int a = 3;
    char param[10];
    sprintf(param, "%d", a);

    string path = "./testcases/process.txt";
    cout << param << endl;
    execl("./process", param , "/mnt/c/Users/ABDULLAH/Desktop/322_1/testcases/process.txt", NULL);

    wait(NULL);
    return 0;
}
