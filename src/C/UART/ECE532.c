/*
 * ECE532.c
 *
 *  Created on: Mar 19, 2017
 *      Author: Anthony
 */

#include <stdio.h>

#include "ECE532.h"
#include "axi_uartlite.h"
#include "xparameters.h"

int str_equ(char *str1, char *str2, int str_len)
{
	int equ_str = 1;
	for(int i = 0 ; i <= str_len ; i++)
	{
		if(str1[i] != str2[i])
		{
			equ_str = 0;
			break;
		}
	}

	return equ_str;
}

void str_cpy(char *str_c, char *str_o, int c_len, int o_len)
{
	int len = 0;

	if(c_len < o_len)
	{
		len = c_len;
	}
	else
	{
		len = o_len;
	}

	for(int i = 0 ; i <= len ; i++)
	{
		str_c[i] = str_o[i];
	}

	return;
}

int str_null(char *str, int str_len)
{
	int null_str = 1;

	for(int i = 0 ; i <= str_len ; i++)
	{
		if(str[i] != '\0')
		{
			null_str = 0;
			break;
		}
	}

	return null_str;
}

void ECE532_unsigned_int_to_string(unsigned int u_i, char *str, char *temp)
{
	int i = 0;

	while(u_i > 0)
	{
		temp[i] = (u_i % 10) + '0';
		u_i /= 10;
		i++;
	}
	i--;

	for(int j = i ; j >=0 ; j--)
	{
		str[i-j] = temp[j];
	}
}

int ECE532_determine_size_from_HPC(char *str, int str_len)
{
	int fsize = 0;

	if(str_len >= MIN_STR_LEN_ECE532_HPC)
	{
		if(str[0] == 'F' && str[1] == 'S' &&
			str[2] == 'I' && str[3] == 'Z' &&
			str[4] == 'E' && str[5] == '=')
		{
			fsize = ECE532_char_dig_2_int(str);
		}
	}

	return fsize;
}

int ECE532_char_dig_2_int(char *str)
{
	int ret_val = 0, i = ECE532_HPC_NUM_START_I;

	while(str[i] != '\0')
	{
		if(str[i] <= '9' || str[i] >= '0')
		{
			ret_val *= 10;
			ret_val += str[i] - 48;
		}
		i++;
	}

	return ret_val;
}

void send_ack(void)
{
	clear_TX_FIFO();
	char buffer[2];
	buffer[0] = (char)0x06;
	buffer[1] = '\0';
	wr_TX_FIFO(buffer, 2);
}

int wait_for_ack(void)
{
	int ack_received = 0;
	char buffer[16+1] = {0};
	while(!ack_received)
	{
		rd_RX_FIFO(buffer, 16);

		for( int i = 0 ; i < 16 ; i++)
		{
			if(buffer[i] == (char)0x06)
			{
				ack_received = 1;
				break;
			}
		}
	}
	return ack_received;
}

int looking_for_eof(char *buffer)
{
	int eof_reached = 0;
	if( (buffer[0] == (char)-1) && (buffer[1] == 'A') )
	{
		eof_reached = 1;
	}
	return eof_reached;
}

int collect_and_store_from_host_PC(volatile int *mem_addr, char *buffer, int buffer_size)
{
	int file_size = 0, eof_reached = 0;
	while(!eof_reached)
	{
		rd_RX_FIFO(buffer, buffer_size);

		eof_reached = looking_for_eof(buffer);

		if(!eof_reached)
		{
			if(!str_null(buffer, buffer_size))
			{
				int i = 0;
				while(i < buffer_size)
				{
					int val = ((unsigned char)buffer[i] << 0) +
								((unsigned char)buffer[i+1] << 8) +
								((unsigned char)buffer[i+2] << 16) +
								((unsigned char)buffer[i+3] << 24);
					*mem_addr = val;
					mem_addr++;
					i += 4;
				}
			}
			send_ack();
		}
	}

	while(!file_size)
	{
		rd_RX_FIFO(buffer, buffer_size);

		if(buffer[0] != '\0')
		{
			file_size = ECE532_determine_size_from_HPC(buffer, buffer_size);
		}
	}
	send_ack();

	return file_size;
}

void send_garbage_to_host_PC(void)
{
	char buffer[16+1];
	for(int i = 0; i < 16 ; i++)
	{
		buffer[i] = 'A';
	}
	buffer[16] = '\0';
	wr_TX_FIFO(buffer, 16);
}

int send_new_file_size_to_host_PC(unsigned int n_file_size, char *buffer, int buffer_size)
{
	int ack_found = 0;
	char str_fs[10+1] = {0}, temp[10+1] = {0};
	if(n_file_size <= 1000000000)
	{
		ECE532_unsigned_int_to_string(n_file_size, str_fs, temp);

		send_garbage_to_host_PC();

		clear_TX_FIFO();

		buffer[0] = 'F';
		buffer[1] = 'S';
		buffer[2] = 'I';
		buffer[3] = 'Z';
		buffer[4] = 'E';
		buffer[5] = '=';

		int i = 0;
		while(str_fs[i] != '\0')
		{
			buffer[6+i] = str_fs[i];
			i++;
		}
		buffer[6+i] = '\0';

		while(!ack_found)
		{
			wr_TX_FIFO(buffer, 6+i);

			ack_found = wait_for_ack();
		}
	}
	else
	{
		ack_found = -1;
	}

	return ack_found;
}

void send_from_mem_to_host_PC(volatile int *mem_addr, int file_size, char *buffer, int buffer_size)
{
	int file_i = 0;
	while(file_i < file_size)
	{
		int i = 0;
		while(i < buffer_size)
		{
			int val = *mem_addr;
			mem_addr++;

			buffer[i] = val & 0xFF;
			buffer[i+1] = (val >> 8) & 0xFF;
			buffer[i+2] = (val >> 16) & 0xFF;
			buffer[i+3] = (val >> 24) & 0xFF;

			i += 4;
		}
		buffer[buffer_size] = '\0';
		file_i += buffer_size;

		wr_TX_FIFO(buffer, buffer_size);

		wait_for_ack();
	}
}
