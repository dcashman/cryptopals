/* quick program to calculate character frequency for ascii values. */

#include <fstream>
#include <iostream>
#include <string>

#include <errno.h>

int main() {
    const char *filename = "mobydick.txt";
    std::ifstream infile;
    infile.open(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Unable to open " << filename << " err: " << strerror(errno) << "\n";
        return -1;
    }
    char c;
    int counts[256];
    memset(counts, 0, 256);
    while (infile.get(c))
        counts[c]++;
    int total = 0;
    for (int i = 0; i < 256; i++)
        total += counts[i];

    std::cout << "Frequencies of each index:\n";
    for (int i = 0; i < 256; i++)
        std::cout << i << ": " << counts[i]/(double) total << "\n";
    return 0;
}
