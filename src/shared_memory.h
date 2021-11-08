#ifndef SHARED_MEMORY
#define SHARED_MEMORY

#include <stdbool.h>

#define BLOCK_SIZE 4096

char *attach_memory_block(char *filename, int size);
bool detach_memory_block(char *block);
bool destroy_memory_block(char *filename);

#define SEM_WRITER_FNAME "/running_writer"
#define SEM_READER_FNAME "/running_reader"

#endif