#ifndef UTILS
#define UTILS

#include <time.h>
#include <sys/types.h>

#include "parse_args.h"

void create_if_not_exist(const char *filename);
void log_pids(pid_t parent, pid_t child[3], arg_data args);
struct timespec get_time();
double get_time_diff(struct timespec st, struct timespec et);

#endif