#include "main.h"

#pragma import(__use_no_semihosting_swi)


extern int sendchar (int c);
extern int getkey   (void);


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stderr = {EXT_DEBUG_HANDLE};
FILE __stdin;
FILE __stdout = {STDOUT_HANDLE};

int fputc(int c, FILE *f) {
	int ret = EOF;
	
	switch(f->handle)
	{
		case STDOUT_HANDLE:
			uart2_putchar_ext_debug(c);
			//ret = uart0_putchar(c); //activate to get debug information from uart0
			break;
		case EXT_DEBUG_HANDLE:
			ret = uart2_putchar_ext_debug(c);
			break;
		default:
			break;
	}
	
  return ret;
}


int fgetc(FILE *f) {
  //return (getkey());
//  return (SER_GetChar());
	return 0;
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int c) {
  //sendchar(c);
//  SER_PutChar(c);
}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
