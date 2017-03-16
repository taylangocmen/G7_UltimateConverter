/*
	Anthony De Caria - 999113659
	
	Copied From: 
*/
#include <stdio.h>
#include "platform.h"
#include "xparameters.h"

// Pointer to the external memory
volatile unsigned int * memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR;

int main()
{
	init_platform();

	int i, errors;

	// Read TEST_SIZE words to memory and compare with golden values
	print("BEGIN READ\n\r");
	errors = 0;
	for (i = 0; i < TEST_SIZE; i++)
	{
		if (memptr[i] != )
			errors++;
	}

	// Print Results
	if (errors != 0)
		print("ERROR FOUND\n\r");
	else
		print("ALL GOOD!\n\r");
	return 0;
}
