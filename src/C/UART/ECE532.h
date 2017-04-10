/*
 * ECE532.h
 *
 *  Created on: Mar 19, 2017
 *      Author: Anthony
 */

#ifndef ECE532_H_
#define ECE532_H_

#define MIN_STR_LEN_ECE532_HPC 8
#define ECE532_HPC_NUM_START_I 6

/**
 * Sees if two strings are equal
 *
 * @params: str1, str2, str_len
 * @returns: 1 if they are equal, 0 if they are not
 */
int str_equ(char *str1, char *str2, int str_len);

/**
 * Copies str_o into str_c
 * Either all of it if str_c is big enough
 * Or the c_len amount.
 *
 * @params: str_c, str_c, c_len, o_len
 * @returns: N/A
 */
void str_cpy(char *str_c, char *str_o, int c_len, int o_len);

/**
 * Sees if a str is NULL
 * I.e. every character is NULL
 *
 * @params: str, str_len
 * @returns: 1 if true, 0 if false
 */
int str_null(char *str, int str_len);

/**
 * Is function is just for ECE532
 * If the string sent is "FSIZE=x", where x is a value
 * Return that x
 *
 * @params: str, str_len
 * @returns: x, 0 if string isn't there
 */
int ECE532_determine_size_from_HPC(char *str, int str_len);

/**
 * Is function is just for ECE532
 * Takes the string from ECE532_determine_size_from_HPC
 * And extracts the x
 *
 * @params: str
 * @returns: x, 0 if string isn't there
 */
int ECE532_char_dig_2_int(char *str);

/**
 * Is function is just for ECE532
 * Takes a integer and turns it into a string
 *
 * @params: u_i, str
 * @returns: N/A
 */
void ECE532_unsigned_int_to_string(unsigned int u_i, char *str, char *temp);

/**
 * Sends an acknowlegdement signal back to the Host PC
 *
 * @params: void
 * @returns: void
 */
void send_ack(void);

/**
 * Polls until the Host PC sends an ACK signal
 *
 * @params: void
 * @returns: 1 if ack received, 0 if not received
 */
int wait_for_ack(void);

/**
 * Collects data from the Host PC
 * and loads it into memory
 *
 * @params: mem_addr, buffer, buffer_size
 * @returns: The size of the data stored in memory
 */
int collect_and_store_from_host_PC(volatile int *mem_addr, char *buffer, int buffer_size);

/**
 * Sends to the Host PC
 * the size of the new file
 *
 * @params: n_file_size, buffer, buffer_size
 * @returns: 0 on failure, 1 on success
 */
int send_new_file_size_to_host_PC(unsigned int n_file_size, char *buffer, int buffer_size);

/**
 * Sees if the buffer sent
 * has the eof string
 *
 * @params: buffer
 * @returns: 0 on failure, 1 on success
 */
int looking_for_eof(char *buffer);

/**
 * Sends data from memory to the host PC
 *
 * @params: mem_addr, mem_size, buffer, buffer_size
 * @returns: void
 */
void send_from_mem_to_host_PC(volatile int *mem_addr, int mem_size, char *buffer, int buffer_size);

/**
 * Sends garbage data to the host PC
 *
 * @params: void
 * @returns: void
 */
void send_garbage_to_host_PC(void);


#endif /* ECE532_H_ */
