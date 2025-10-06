#ifndef system_call_header
#define system_call_header

#include <stdint.h> //for type definitions

void print_system_call(char* string, uint32_t bytes);
void exit_system_call();
int fork_system_call();

#endif