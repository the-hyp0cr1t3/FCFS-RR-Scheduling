#include <fstream>
#include <iostream>
#include <random>

using namespace std;

#define MAX_VAL (int)(1e6)

void make_txt(string fname, int n) {
    fstream file;
    file.open(fname, ios::out | ios::trunc);

    for (int i = 0; i < n; ++i) {
        file << (rand() % MAX_VAL) + 1 << "\n";
    }
    file.close();
}

int main(int argc, char* argv[]) {
    // First Argument to Executable = file size
    // Second Argument = File Name

    srand(42);

    int n2, n3;
    cout << "Number of Elements Required in c2.txt: ";
    cin >> n2;
    make_txt("c2.txt", n2);
    cout << "c2.txt created!\n";

    cout << "Number of Elements Required in c3.txt: ";
    cin >> n3;

    make_txt("c3.txt", n3);
    cout << "c3.txt created!\n";
}