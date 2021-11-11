#ifndef UTILS
#define UTILS

#include <time.h>

void check_file(char *filename);

double get_time_diff(struct timespec st, struct timespec et);

#endif