#ifndef SCHEDULING
#define SCHEDULING

#define SHM_CURRENT_RUNNING_FILENAME "current_running.shm"
#define SHM_DONE_FILENAME "done.shm"

void* monitor(void *args);
void* worker(void *args);
void child_method(int process_id);

#endif