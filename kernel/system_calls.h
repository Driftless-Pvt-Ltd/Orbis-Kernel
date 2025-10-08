#ifndef system_call_header
#define system_call_header

#include <stdint.h> //for type definitions

void exit_system_call();
int fork_system_call();
void write_system_call(char* name, uint32_t bytes);

#endif