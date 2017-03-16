/*
	Anthony De Caria - March 10, 2017
	
	This code allows the Microblaze processor to access the AXI UARTLITE
*/

#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include <stdio.h>

#include "axi_uartlite.h"

char rd_byte_reg(volatile int *reg_address)
{
	char rd_byte = *reg_address;
	
	return rd_byte;
}

void wr_byte_reg(volatile int *reg_address, char data)
{
	if(reg_address == (volatile int *)TX_FIFO)
	{
		wait_for_FIFO(TX_FIFO);
	}

	*reg_address = data;
	return;
}

char rd_status(void)
{
	return rd_byte_reg(STATUS);
}

int can_we_read_RX(void)
{
	char status = rd_status();
	
	return status & RX_VALID_BT;
}

char rd_RX_byte(void)
{
	return rd_byte_reg(RX_FIFO);
}

int rd_RX_FIFO(char *collected_data, int max_data_length)
{
	int i = 0;
	
	while(i < max_data_length)
	{
		wait_for_FIFO(RX_FIFO);

		char t = rd_RX_byte();
		if(t != 0xff)
		{
			collected_data[i] = t;
			i++;
		}
		else
		{
			break;
		}
	}
	return i;
}

int can_we_write_TX(void)
{
	char status = rd_status();

	int return_val = ((status & TX_FULL_BT) >> TX_FULL_BT_SHIFT_VAL);

	if(return_val)
	{
		return_val = 0;
	}
	else
	{
		return_val = 1;
	}

	return return_val;
}

void wait_for_FIFO(int fifo_type)
{
	int status_value = 0xffff;

	if(fifo_type == RX_FIFO)
	{
		status_value = can_we_read_RX();
	}
	else if (fifo_type == TX_FIFO)
	{
		status_value = can_we_write_TX();
	}

	while(!status_value)
	{
		for(int i = 0 ; i < STALL_VALUE ; i++)
			; //do nothing

		if(fifo_type == RX_FIFO)
		{
			status_value = can_we_read_RX();
		}
		else if (fifo_type == TX_FIFO)
		{
			status_value = can_we_write_TX();
		}
	}

	return;
}

void wr_TX_byte(char byte_to_send)
{
	wait_for_FIFO(TX_FIFO);

	wr_byte_reg(TX_FIFO, byte_to_send);
	return;
}

int wr_TX_FIFO(char *string_to_send, int string_len)
{
	int i = 0;
	
	while(i < string_len)
	{
		wait_for_FIFO(TX_FIFO);

		wr_TX_byte(string_to_send[i]);
		i++;
	}
	return i;
}

void wr_control_reg(char data)
{
	wr_byte_reg(CONTROL, data);
	return;
}


