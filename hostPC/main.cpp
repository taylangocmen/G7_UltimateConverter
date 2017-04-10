/*
	March 3, 2017
	
	This code was found @ http://playground.arduino.cc/Interfacing/CPPWindows
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cstddef>

#include <math.h>

#include "WinSerialClass.h"

using namespace std;

#define BUFFER_SIZE 16
#define DISPLAY_SIZE 2048
#define TX_TIME_DELAY 100
#define RX_TIME_DELAY 1100

int main(void)
{
	char *port_name = new char[5];
	strncpy(port_name, "COM5", 5);
	Serial FPGA(port_name);
	
	char PC_to_FPGA[BUFFER_SIZE+1] = {0}, FPGA_to_PC[BUFFER_SIZE+1] = {0}, blank[BUFFER_SIZE+1] = {0};
	char Display[DISPLAY_SIZE+1] = {0};

	bool write_success = true;

	unsigned int packets_sent = 0, valid_packets = 0, display_i = 0;

	ifstream test_text_file;
	test_text_file.open("C:/Users/Anthony/workspace/FPGA_PC_ECE532/src/smallimage.bmp", ios::in|ios::binary|ios::ate);
	size_t ttf_size = test_text_file.tellg();
	test_text_file.seekg(0, ios::beg);
	char *ttf_buffer = new char [ttf_size+1] ;
	test_text_file.read(ttf_buffer, ttf_size);
	ttf_buffer[ttf_size] = '\0';

	if (test_text_file.is_open())
	{
//		for(int i = 0; i < BUFFER_SIZE ; i++)
//		{
//			PC_to_FPGA[i] = 'A';
//		}
//		write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
//		if (write_success == false)
//		{
//			cout << "Error sending line." << endl;
//			return -1;
//		}
//		cout << "Packet was: " << PC_to_FPGA << endl;
//		strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
//		Sleep(TX_TIME_DELAY);
//		bool ack_found = false;
//		while(!ack_found)
//		{
//			int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE);
//			if (read_success != -1 || read_success != 0)
//			{
//				for(int i = 0 ; i < BUFFER_SIZE ; i++)
//				{
//					if(FPGA_to_PC[i] == (char)0x06)
//					{
//						ack_found = true;
//						break;
//					}
//				}
//				strncpy(FPGA_to_PC, blank, BUFFER_SIZE);
//			}
//			else
//			{
//				cout << "Read Error" << endl;
//				return -1;
//			}
//		}

		while( packets_sent*BUFFER_SIZE < ttf_size) // We still have to send things
		{
			for(int i = 0; i < BUFFER_SIZE ; i++)
			{
				PC_to_FPGA[i] = ttf_buffer[packets_sent*BUFFER_SIZE + i];
			}
			write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
			if (write_success == false)
			{
				cout << "Error sending line." << endl;
				return -1;
			}
			cout << "Sent packet " << packets_sent << endl;
//			cout << "Packet was: " << PC_to_FPGA << endl;
			for(int i = 0 ; i < BUFFER_SIZE ; i++)
			{
				printf("%02X", (unsigned char)PC_to_FPGA[i]);
			}
			cout << endl;
			strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
			packets_sent++;
			Sleep(TX_TIME_DELAY);
			bool ack_found = false;
			while(!ack_found)
			{
				int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE);
				if (read_success != -1 || read_success != 0)
				{
					for(int i = 0 ; i < BUFFER_SIZE ; i++)
					{
						if(FPGA_to_PC[i] == (char)0x06)
						{
							ack_found = true;
							break;
						}
					}
					strncpy(FPGA_to_PC, blank, BUFFER_SIZE);
				}
				else
				{
					cout << "Read Error" << endl;
					return -1;
				}
			}
		}
	}
	else
	{
		cout << "Error reading file." << endl;
		return -1;
	}
	test_text_file.close();

	PC_to_FPGA[0] = EOF;
	PC_to_FPGA[1] = 'A';
	write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
	if (write_success == false)
	{
		cout << "Error sending data." << endl;
		return -1;
	}
	packets_sent++;
	cout << PC_to_FPGA << endl;
	strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
	Sleep(1000);

	bool ack_found = false;
	while(!ack_found)
	{
		int digits_in_ttf_size = (int)log10(ttf_size) + 1;
		char *temp = new char[digits_in_ttf_size + 1];
		itoa((int)ttf_size, temp, 10);
		PC_to_FPGA[0] = 'F';
		PC_to_FPGA[1] = 'S';
		PC_to_FPGA[2] = 'I';
		PC_to_FPGA[3] = 'Z';
		PC_to_FPGA[4] = 'E';
		PC_to_FPGA[5] = '=';
		for(int i = 0 ; i < digits_in_ttf_size ; i++ )
		{
			PC_to_FPGA[i+6] = temp[i];
		}
		PC_to_FPGA[6+digits_in_ttf_size] = '\0';
		write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
		if (write_success == false)
		{
			cout << "Error sending data." << endl;
			return -1;
		}
		strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
		cout << "Sent file size" << endl;
		Sleep(3000);

		int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE);
		if (read_success != -1 || read_success != 0)
		{
			if(FPGA_to_PC[0] != '\0')
			{
				cout << FPGA_to_PC << endl;
			}
			for(int i = 0 ; i < BUFFER_SIZE ; i++)
			{
				if(FPGA_to_PC[i] == (char)0x06)
				{
					ack_found = true;
					cout << "File size ack received" << endl;
					break;
				}
			}
			strncpy(FPGA_to_PC, blank, BUFFER_SIZE);
		}
		else
		{
			cout << "Read Error" << endl;
			return -1;
		}
	}

//	while(true)
//	{
//		int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE);
//		if (read_success != -1 || read_success != 0)
//		{
//			if(FPGA_to_PC[0] != '\0')
//			{
//				cout << "Got " << FPGA_to_PC << endl;
//			}
//		}
//		Sleep(2000);
//	}

	unsigned int new_file_size = 0;
	while(!new_file_size)
	{
		int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE+1);
		if (read_success != -1 || read_success != 0)
		{
			if(FPGA_to_PC[0] != '\0')
			{
				cout << "NFS received was " << FPGA_to_PC << endl;
				if(sizeof(FPGA_to_PC) >= 6)
				{
					if(FPGA_to_PC[0] == 'F' && FPGA_to_PC[1] == 'S' &&
						FPGA_to_PC[2] == 'I' && FPGA_to_PC[3] == 'Z' &&
						FPGA_to_PC[4] == 'E' && FPGA_to_PC[5] == '=')
					{
						int i = 6;
						while(FPGA_to_PC[i] != '\0')
						{
							if(FPGA_to_PC[i] <= '9' || FPGA_to_PC[i] >= '0')
							{
								new_file_size *= 10;
								new_file_size += FPGA_to_PC[i] - 48;
							}
							i++;
						}
					}
				}
			}
			strncpy(FPGA_to_PC, blank, BUFFER_SIZE);
		}
		else
		{
			cout << "Read Error" << endl;
			return -1;
		}
	}
	PC_to_FPGA[0] = (char)0x06;
	write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
	if (write_success == false)
	{
		cout << "Error sending data." << endl;
		return -1;
	}
	strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
	Sleep(4000);

	while(valid_packets*BUFFER_SIZE < new_file_size)
	{
		int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE+1);
		if (read_success != -1 || read_success != 0)
		{
//			if(FPGA_to_PC[0] != '\0')
//			{
				cout << "Received packet " << valid_packets << endl;
//				cout << "Packet was: " << FPGA_to_PC << endl;
				for(int i = 0 ; i < BUFFER_SIZE ; i++)
				{
					printf("%02X", (unsigned char)FPGA_to_PC[i]);
				}
				cout << endl;
				int i = 0;
				for( ; i < BUFFER_SIZE ; i++)
				{
//					if(FPGA_to_PC[i] != '\0')
//					{
//						if(FPGA_to_PC[i] == 'A' && valid_packets == 0)
//							display_i--;
//						else
							Display[display_i + i] = FPGA_to_PC[i];
//					}
//					else
//					{
//						break;
//					}
				}
				display_i += i;

				PC_to_FPGA[0] = (char)0x06;
				write_success = FPGA.WriteData(PC_to_FPGA, BUFFER_SIZE);
				if (write_success == false)
				{
					cout << "Error sending data." << endl;
					return -1;
				}
				strncpy(PC_to_FPGA, blank, BUFFER_SIZE);
				valid_packets++;
				Sleep(4000);
//			}
			strncpy(FPGA_to_PC, blank, BUFFER_SIZE);
		}
		else
		{
			cout << "Read Error" << endl;
			return -1;
		}
	}

	cout << "Sent " << packets_sent << " packets." << endl;
	cout << "Received " << valid_packets << " valid packets." << endl;
//	cout << Display << endl;

	ofstream fout;
	fout.open("final.bmp", ios::binary | ios::out);
	fout.write(Display, new_file_size);
	fout.close();

	return 0;
}
