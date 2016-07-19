#include <iostream>

#include "Analysis.h"

#include "matasano_set01.h"

#define MAX_PROBLEMS 8

std::string (*matasano_problems[MAX_PROBLEMS])() = {
    problem01,
    problem02,
    problem03,
    problem04,
    problem05,
    problem06,
    problem07,
    problem08,
};

static void usage() {
    std::cout << "Usage: matasano <root_dir> <problem_num>\n";
    exit(0);
}

std::string rootdir;
std::vector<double> english_freq_table;

/* matasano - solve the matasano challenges. */
int main(int argc, char** argv) {
    if (argc != 3)
        usage();
    rootdir = std::string(argv[1]);
    int problem_num = atoi(argv[2]);
    if (problem_num < 0 || problem_num > MAX_PROBLEMS )
        usage();
    // get english ascii frequency distribution (based on moby dick).
    english_freq_table = create_ascii_freq_table(rootdir + "res/mobydick.txt");

    std::cout << "matasano problem " << problem_num << " " << matasano_problems[problem_num - 1]() << "\n";

    return 0;
}
