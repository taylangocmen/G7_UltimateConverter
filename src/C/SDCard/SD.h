/*
 * SD.h
 *
 *  Created on: Feb 28, 2017
 */

#ifndef SD_H_
#define SD_H_

#include <stdbool.h>
#include "xintc.h"
#include "xgpio.h"

/*
	Constants
*/
// SD High Level
#define SD_GPIO_CARD_PRESENT_MASK (0x00000001)
#define SD_GPIO_DEVICE_ID   XPAR_GPIO_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID

#define SD_SETUP_FAILURE -1

/*
	Functions
*/
extern XIntc InterruptController;
extern XGpio SdGpio;

static int SetupInterruptSystem(void);
int SetupSdGpio(void);
bool SdCardInserted(void);
void wait_for_SD_Card(void);
FRESULT scan_files (char* path);


// SD Test Suites

static void LowLevelTest(void);
static void HighLevelTest(void);


#endif /* SD_H_ */
