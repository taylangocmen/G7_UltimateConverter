/*
	Anthony De Caria - March 10, 2017
	
	This code allows the Microblaze processor to access the AXI UARTLITE
*/

#ifndef AXI_UARTLITE_H_ 
#define AXI_UARTLITE_H_

#define RX_FIFO	0x40600000
#define TX_FIFO	0x40600004
#define STATUS	0x40600008
#define CONTROL	0x4060000C

#define RX_VALID_BT		0x01
#define RX_FULL_BT		0x02
#define TX_EMTY_BT		0x04
#define TX_FULL_BT		0x08
#define INTR_EN_BT		0x10
#define OVERUN_ERR_BT	0x20
#define FRAME_ERR_BT	0x40
#define PARITY_ERR_BT	0x80

#define RST_TX_BT	0x01
#define RST_RX_BT	0x02
#define EN_INTR_BT	0x10

#define STALL_VALUE 10000000
#define TX_FULL_BT_SHIFT_VAL 3

/**
	Reads a byte to a general register
	
	@params: reg_address
	@returns: the byte if successful, 0xff if failure
*/
char rd_byte_reg(volatile int *reg_address);

/**
	Write a byte to a general register
	
	@params: reg_address, data
	@returns: N/A
*/
void wr_byte_reg(volatile int *reg_address, char data);

/**
	Reads the status register
	
	@params: N/A
	@returns: the byte if successful, 0xff if failure
*/
char rd_status(void);

/**
	Determines if we can read data from the RX FIFO
	
	@params: N/A
	@returns: 1 if we can read, 0 if we cannot
*/
int can_we_read_RX(void);

/**
	Determines if we can write data to the TX FIFO
	
	@params: N/A
	@returns: 1 if we can read, 0 if we cannot
*/
int can_we_write_TX(void);

/**
	Waits for either FIFO to settle

	@params: fifo_type
	@returns: N/A
*/
void wait_for_FIFO(int fifo_type);

/**
	Reads one byte from the RX FIFO
	Note: this just reads the line - it does NOT make sure status is correct
	
	@params: N/A
	@returns: the byte if successful, 0xff if failure
*/
char rd_RX_byte(void);

/**
	Reads all the RX FIFO data
	
	@params: collected_data, max_data_length
	@returns: number of bytes read
*/
int rd_RX_FIFO(char *collected_data, int max_data_length);

/**
	Write a byte the TX FIFO register
	
	@params: byte_to_send
	@returns: N/A
*/
void wr_TX_byte(char byte_to_send);

/**
	Writes a string to the TX FIFO register
	
	@params: string_to_send, string_len
	@returns: number of bytes read
*/
int wr_TX_FIFO(char *string_to_send, int string_len);

/**
	Writes a byte to the control reg
	
	@params: data
	@returns: N/A
*/
void wr_control_reg(char data);

/**
	Clears both FIFOs

	@params: N/A
	@returns: N/A
*/
void clear_FIFOs(void);

/**
	Clears the RX FIFO

	@params: N/A
	@returns: N/A
*/
void clear_RX_FIFO(void);

/**
	Clears the TX FIFO

	@params: N/A
	@returns: N/A
*/
void clear_TX_FIFO(void);

#endif // AXI_UARTLITE_H_
