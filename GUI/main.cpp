/*
	March 3, 2017
	
	This code was found @ http://playground.arduino.cc/Interfacing/CPPWindows
*/

#include <iostream>
#include <string>

#include "WinSerialClass.h"

using namespace std;

#define BUFFER_SIZE 255

int main(void)
{
	char *port_name = new char[5];
	strncpy(port_name, "COM5", 5);

	Serial FPGA(port_name);
	
	char PC_to_FPGA[BUFFER_SIZE+1] = {0}, FPGA_to_PC[BUFFER_SIZE+1] = {0};
	string temp = "";

	cout << "Please enter a string to send to the FPGA." << endl;
	getline (cin, temp);
	strncpy(PC_to_FPGA, temp.c_str(), temp.size());
	bool write_success = FPGA.WriteData(PC_to_FPGA, temp.size());
	if (write_success == false)
	{
		cout << "Error sending data." << endl;
		return -1;
	}

	Sleep(10000);

	int read_success = FPGA.ReadData(FPGA_to_PC, BUFFER_SIZE);
	if (read_success == -1)
	{
		cout << "Error receiving data." << endl;
		return -1;
	}
	else if (read_success == 0)
	{
		cout << "No data received." << endl;
	}
	else
	{
		cout << FPGA_to_PC << endl;
	}
	return 0;
}
