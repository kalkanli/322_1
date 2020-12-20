#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <map>

using namespace std;

map<int, int> process_ids = {};
int main(int argc, char const *argv[])
{
    int id1 = 1234;
    int id2 = 1235;
    int a = 3;
    int b = 3;

    process_ids.insert(pair<int, int>(a, id1));

    if(3 == 3) {
        process_ids.insert(pair<int,int>(2, 154));
    }
    map<int, int>::iterator pair;

    pair = process_ids.find(b);
    cout << id1 << endl;
    pair = process_ids.find(154);
    cout << pair->second << endl;

    // char param[10];
    // sprintf(param, "%d", a);

    // string path = "./testcases/process.txt";
    // cout << param << endl;
    // execl("process", param , "/mnt/c/Users/ABDULLAH/Desktop/322_1/testcases/process.txt", NULL);

    // wait(NULL);
    return 0;
}
