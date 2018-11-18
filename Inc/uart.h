/*
 * File:		uart.h
 * Purpose:     Provide common ColdFire uart routines for polled serial IO
 *
 * Notes:
 */
#ifndef UART_H
#define UART_H

// UART clock sources
#define UART_PLLFLL 	1	// UART source is MCGFLLCLK clock or MCGPLLCLK/2
#define UART_OSCERCLK 2
#define UART_MCGIRCLK	3

#define BUS_CLK_KHZ BUS_CLOCK / 1000

void uart_init (UART_MemMapPtr uartch, int sysclk, int baud);
char uart_getchar (UART_MemMapPtr channel);
void uart_putchar (UART_MemMapPtr channel, char ch);
int uart_getchar_present (UART_MemMapPtr channel);
void uart0_init(int baud, int sysclk);
char uart0_getchar(UART0_MemMapPtr channel);
int uart0_putchar(char ch);
int uart0_getchar_present (UART0_MemMapPtr channel);

void uart1_init_bluetooth(int baud);

void uart2_init_ext_debug(int baud);
int uart2_putchar_ext_debug(char ch);
void put_string(int out_channel, char* string);

/********************************************************************/

#endif
