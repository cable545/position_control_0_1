#include "main.h"

/*----------------------------------------------------------------------------------------------*/
/*                                      own print function                                      */
/*----------------------------------------------------------------------------------------------*/
void print(int out_channel, char* string, ...)
{
	char* traverse;
	int i;
	char* s;
	
	va_list arg;
	va_start(arg, string);
	
	for(traverse = string; *traverse != '\0'; traverse++)
	{
		while(*traverse != '%') 
		{ 
			if(*traverse == '\0')
				return;
			
			if(out_channel == STDOUT_HANDLE)
				uart0_putchar(*traverse);
			else if(out_channel == EXT_DEBUG_HANDLE)
				uart2_putchar_ext_debug(*traverse);
			
			traverse++; 
		}

		traverse++;
		
		switch(*traverse)
		{
			case 'c': i = va_arg(arg, int);		//Fetch char argument
				if(out_channel == STDOUT_HANDLE)
					uart0_putchar(i);
				else if(out_channel == EXT_DEBUG_HANDLE)
					uart2_putchar_ext_debug(i);
				
				break;
			case 'i':
			case 'd': i = va_arg(arg, int); 		//Fetch Decimal/Integer argument
				if(i < 0)
				{ 
					i = -i;
					
					if(out_channel == STDOUT_HANDLE)
						uart0_putchar('-');
					else if(out_channel == EXT_DEBUG_HANDLE)
						uart2_putchar_ext_debug('-');
				}
				
				put_string(out_channel, convert(i, 10));
				
				break;	
			case 'o': i = va_arg(arg, unsigned int); //Fetch Octal representation
				put_string(out_channel, convert(i, 8));
				
				break;				
			case 's': s = va_arg(arg, char *); 		//Fetch string
				put_string(out_channel, s);
				
				break;
			case 'x': i = va_arg(arg, unsigned int); //Fetch Hexadecimal representation
				put_string(out_channel, convert(i,16));
				
				break;
		}
	}
	
	va_end(arg);
}

char* convert(unsigned int num, int base)
{ 
	char Representation[]= "0123456789ABCDEF";
	static char buffer[50] = {0};
	char *ptr;
	
	ptr = &buffer[49];
	*ptr = '\0';
	
	do
	{
		*--ptr = Representation[num % base];
		num /= base;
	}while(num != 0);
	
	return ptr;
}
