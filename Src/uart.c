#include "main.h"

/********************************************************************/
/*
 * Initialize the uart for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 * NOTE: Since the uarts are pinned out in multiple locations on most
 *       Kinetis devices, this driver does not enable uart pin functions.
 *       The desired pins should be enabled before calling this init function.
 *
 * Parameters:
 *  uartch      uart channel to initialize
 *  sysclk      uart module Clock in kHz(used to calculate baud)
 *  baud        uart baud rate
 */
void uart_init(UART_MemMapPtr uartch, int sysclk, int baud)
{
	register uint16 sbr;
	uint8 temp;
															
	if (uartch == UART1_BASE_PTR)
		SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
	else
	SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
	
	/* Make sure that the transmitter and receiver are disabled while we 
	* change settings.
	*/
	UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK );
	
	/* Configure the uart for 8-bit mode, no parity */
	UART_C1_REG(uartch) = 0;	/* We need all default settings, so entire register is cleared */
	
	/* Calculate baud settings */
	sbr = (uint16)((sysclk * 1000) / (baud * 16));
		
	/* Save off the current value of the uartx_BDH except for the SBR field */
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));
	
	UART_BDH_REG(uartch) = temp |  UART_BDH_SBR(((sbr & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8)(sbr & UART_BDL_SBR_MASK);
	
	/* Enable receiver and transmitter */
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}
/********************************************************************/
/*
 * Wait for a character to be received on the specified uart
 *
 * Parameters:
 *  channel      uart channel to read from
 *
 * Return Values:
 *  the received character
 */
char uart_getchar(UART_MemMapPtr channel)
{
	/* Wait until character has been received */
	while (!(UART_S1_REG(channel) & UART_S1_RDRF_MASK));
	
	/* Return the 8-bit data from the receiver */
	return UART_D_REG(channel);
}
/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  channel      uart channel to send to
 *  ch			 character to send
 */ 
void uart_putchar(UART_MemMapPtr channel, char ch)
{
	/* Wait until space is available in the FIFO */
	while(!(UART_S1_REG(channel) & UART_S1_TDRE_MASK));
	
	/* Send the character */
	UART_D_REG(channel) = (uint8)ch; 
 }
/********************************************************************/
/*
 * Check to see if a character has been received
 *
 * Parameters:
 *  channel      uart channel to check for a character
 *
 * Return values:
 *  0       No character received
 *  1       Character has been received
 */
int uart_getchar_present(UART_MemMapPtr channel)
{
	return (UART_S1_REG(channel) & UART_S1_RDRF_MASK);
}
/********************************************************************/

/***************************************************************************
 * Begin UART0 functions
 **************************************************************************/
/********************************************************************/
/*
 * Initialize the uart for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 * NOTE: Since the uarts are pinned out in multiple locations on most
 *       Kinetis devices, this driver does not enable uart pin functions.
 *       The desired pins should be enabled before calling this init function.
 *
 * Parameters:
 *  sysclk      uart module Clock in Hz(used to calculate baud) for UART0 it is the pll clk/2 => 48Mhz/2 24Mhz
 *  baud        uart baud rate
 */
void uart0_init(int baud, int sysclk)
{
	uint8 i;
  uint32 calculated_baud = 0;
  uint32 baud_diff = 0;
  uint32 osr_val = 0;
  uint32 sbr_val;
  uint32 baud_rate;
  uint32 reg_temp = 0;
  uint32 temp = 0;
	
	PORTA_PCR1 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTA_PCR1 to UART0
	PORTA_PCR2 = PORT_PCR_MUX(2) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTA_PCR2 to UART0
	
	SIM_SOPT2 |= SIM_SOPT2_UART0SRC(UART_PLLFLL); //set the pll with a divider of 2 as clk source
	SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
    
	// Disable UART0 before changing registers
	UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
  
	// Verify that a valid clock value has been passed to the function 
	if ((sysclk > 50000000) || (sysclk < 32000))
	{
		sysclk = 0;
		reg_temp = SIM_SOPT2;
		reg_temp &= ~SIM_SOPT2_UART0SRC_MASK;
		reg_temp |= SIM_SOPT2_UART0SRC(0);
		SIM_SOPT2 = reg_temp;
			
		// Enter inifinite loop because the 
		// the desired system clock value is 
		// invalid!!
		while(1) {}
	}
  
	// Initialize baud rate
	baud_rate = baud;

	// Calculate the first baud rate using the lowest OSR value possible.  
	i = 4;
	sbr_val = (uint32)(sysclk / (baud_rate * i));
	calculated_baud = (sysclk / (i * sbr_val));
        
	if (calculated_baud > baud_rate)
		baud_diff = calculated_baud - baud_rate;
	else
		baud_diff = baud_rate - calculated_baud;
    
	osr_val = i;
        
	// Select the best OSR value
	for (i = 5; i <= 32; i++)
	{
		sbr_val = (uint32)(sysclk / (baud_rate * i));
		calculated_baud = (sysclk / (i * sbr_val));
        
		if (calculated_baud > baud_rate)
			temp = calculated_baud - baud_rate;
		else
			temp = baud_rate - calculated_baud;
        
		if (temp <= baud_diff)
		{
			baud_diff = temp;
			osr_val = i; 
		}
	}
    
	if (baud_diff < ((baud_rate / 100) * 3))
	{
		// If the OSR is between 4x and 8x then both
		// edge sampling MUST be turned on.  
		if ((osr_val >3) && (osr_val < 9))
			UART0_C5|= UART0_C5_BOTHEDGE_MASK;
        
		// Setup OSR value 
		reg_temp = UART0_C4;
		reg_temp &= ~UART0_C4_OSR_MASK;
		reg_temp |= UART0_C4_OSR(osr_val-1);
    
		// Write reg_temp to C4 register
		UART0_C4 = reg_temp;
        
		reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
		sbr_val = (uint32)((sysclk) / (baud_rate * (reg_temp)));
        
		/* Save off the current value of the uartx_BDH except for the SBR field */
		reg_temp = UART0_BDH & ~(UART0_BDH_SBR(0x1F));
   
		UART0_BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
		UART0_BDL = (uint8)(sbr_val & UART0_BDL_SBR_MASK);
        
		/* Enable receiver and transmitter */
		UART0_C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	}
  else
	{
		// Unacceptable baud rate difference
		// More than 3% difference!!
		// Enter infinite loop!
    while(1) {}
	}					   
}
/********************************************************************/
/*
 * Wait for a character to be received on the specified uart
 *
 * Parameters:
 *  channel      uart channel to read from
 *
 * Return Values:
 *  the received character
 */
char uart0_getchar(UART0_MemMapPtr channel)
{
      /* Wait until character has been received */
      while (!(UART0_S1_REG(channel) & UART0_S1_RDRF_MASK));
    
      /* Return the 8-bit data from the receiver */
      return UART0_D_REG(channel);
}
/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  channel      uart channel to send to
 *  ch			 character to send
 */ 
int uart0_putchar(char ch)
{	
	/* Wait until space is available in the FIFO */
	while(!(UART0_S1 & UART0_S1_TDRE_MASK));
    
	/* Send the character */
	UART0_D |= (uint8)ch;
	
	return (ch & 0xFF);
}
/********************************************************************/
/*
 * Check to see if a character has been received
 *
 * Parameters:
 *  channel      uart channel to check for a character
 *
 * Return values:
 *  0       No character received
 *  1       Character has been received
 */
int uart0_getchar_present(UART0_MemMapPtr channel)
{
	return (UART0_S1_REG(channel) & UART0_S1_RDRF_MASK);
}
/********************************************************************/
/*
 * Inititialize the uart1 module for execution on the frdm kl46z board on porte0 and 1
 *
 * Parameters:
 *  baud      uart baud rate
 */
void uart1_init_bluetooth(int baud)
{
	PORTE_PCR0 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTE_PCR0 to UART1_TX
	PORTE_PCR1 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTE_PCR1 to UART1_RX
	
	uart_init(UART1_BASE_PTR, BUS_CLK_KHZ, baud);
	UART_C2_REG(UART1_BASE_PTR) |= UART_C2_RIE_MASK;
	
	enable_irq(INT_UART1 -16);
	set_irq_priority((INT_UART1 -16), 3);
}
/********************************************************************/
/*
 * Inititialize the uart2 module for execution on the frdm kl46z board on porte16 and 17
 *
 * Parameters:
 *  baud      uart baud rate
 */
void uart2_init_ext_debug(int baud)
{
	PORTE_PCR16 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTE_PCR16 to UART2_TX
	PORTE_PCR17 = PORT_PCR_MUX(3) | PORT_PCR_DSE_MASK; //This sets the Mux control of PORTE_PCR17 to UART2_RX
	
	uart_init(UART2_BASE_PTR, BUS_CLK_KHZ, baud);
}
/********************************************************************/
/*
 * Wait for space in the uart Tx FIFO and then send a character
 *
 * Parameters:
 *  ch			 character to send
 */
int uart2_putchar_ext_debug(char ch)
{
	/* Wait until space is available in the FIFO */
	while(!(UART_S1_REG(UART2_BASE_PTR) & UART_S1_TDRE_MASK));
	
	/* Send the character */
	UART_D_REG(UART2_BASE_PTR) = (uint8)ch;
	
	return (ch & 0xFF);
}
/********************************************************************/
/*
 * put a string to the data stream dependent from the given parameter out_channel
 *
 * Parameters:
 *	out_channel	output channel for the string
 *  string			character to send
 */
void put_string(int out_channel, char* string)
{
	uint32 i = 0;
	
	while(string[i] != '\0')
	{
		if(out_channel == STDOUT_HANDLE)
			uart0_putchar(string[i]);
		else if(out_channel == EXT_DEBUG_HANDLE)
			uart2_putchar_ext_debug(string[i]);
		
		i++;
	}
}
	




