#ifndef DEBUG_H
#define DEBUG_H

enum 
{
  STDOUT_HANDLE,
  EXT_DEBUG_HANDLE
};

void print(int out_channel, char* string, ...);
char* convert(unsigned int num, int base);

#endif
