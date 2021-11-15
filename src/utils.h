#ifndef UTILS
#define UTILS

#include <time.h>
#include <sys/types.h>

#include "context_manager.h"

void check_file(const char *filename);
void log_pids(pid_t parent, pid_t child[3], task ts);

double get_time_diff(struct timespec st, struct timespec et);

#endif