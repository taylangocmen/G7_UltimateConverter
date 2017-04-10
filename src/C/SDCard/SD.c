/*
 * SD.c
 *
 *  Created on: Feb 28, 2017
 */
#include "xil_printf.h"
#include "xparameters.h"
#include "xspi.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xgpio.h"
#include "microblaze_sleep.h"

#include "SD.h"
#include "platform.h"
#include "diskio.h"
#include "ff.h"
#include "string.h"

/*
 * The instances to support the device drivers are global such that the
 * are initialized to zero each time the program runs.  They could be local
 * but should at least be static so they are zeroed.
 */
XIntc InterruptController;
XGpio SdGpio;

static TCHAR Buff[100];

int SetupSdGpio(void)
{
	int Status;
	
	// Initialize the GPIO driver. If an error occurs then exit.
	Status = XGpio_Initialize(&SdGpio, SD_GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Failed to initialize SD GPIO driver: %d\n\r", Status);
		return XST_FAILURE;
	}
	
	// Set the direction for all signals to be inputs.
	XGpio_SetDataDirection(&SdGpio, 1, SD_GPIO_CARD_PRESENT_MASK);
	
	XGpio_SelfTest(&SdGpio);
	
	// TODO: add Interrupt configuration code.
	
	return XST_SUCCESS;
}

void wait_for_SD_Card(void)
{
	if (!SdCardInserted()) 
	{
    	xil_printf("SD card not inserted! Derp!\n\r");
    	xil_printf("Please insert SD card...\n\r");
        while (!SdCardInserted()) 
		{
        	MB_Sleep(100);
        }
    }
}

inline bool SdCardInserted()
{
    int RegRead = XGpio_DiscreteRead(&SdGpio, 1);
    return (RegRead & SD_GPIO_CARD_PRESENT_MASK) == 0;
}

/****************************************************************************
* This function sets up the interrupt system such that interrupts can occur.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int SetupInterruptSystem()
{
	int Status;
	
	/*
	 * Initialize the interrupt controller driver so that
	 * it's ready to use, specify the device ID that is generated in
	 * xparameters.h
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	
	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the SPI can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}
	
	/*
	 * Initialize the exception table
	 */
	Xil_ExceptionInit();
	
	/*
	 * Enable non-critical exceptions
	 */
	Xil_ExceptionEnable();
	
	return XST_SUCCESS;
}

static void LowLevelTest()
{
	u8 TestBuffer[2048];
	
    // Try to initialize the SD card.
    if (disk_initialize(0))
	{
    	xil_printf("Failed to initialize SD card.\n\r");
    	return;
    }
	
    xil_printf("SD card initialized.\n\r");
	
  	if (disk_read(0, TestBuffer, 0, 2) != RES_OK)
	{
    	xil_printf("Failed to read first sector.\n\r");
    	return;
  	}
	
  	for (int i = 0; i < 128; i++)
	{
  		for (int j = 0; j < 4; j++) 
		{
  			xil_printf("%02x ", TestBuffer[4 * i + j]);
  		}
  		xil_printf("\r\n");
  	}
}

static void HighLevelTest(void) {
	FATFS FatFs;
	FIL FHandle;
	unsigned int BytesWritten;
	FRESULT res;
	char buff[256];
	
	res = f_mount(&FatFs, "", 1);
	
	if (res == FR_OK)
	{
		res = scan_files(buff);
	}
	
	char *file_name = "TEXTDOC.TXT";
	
	if (f_open(&FHandle, file_name, FA_READ) != FR_OK)
	{
    	xil_printf("Failed to open %s.\n\r", file_name);
		return;
	}
	
	int size;
	size = f_size(&FHandle);
	xil_printf("size: %d \n",size);
	
	while (!f_eof(&FHandle))
	{
		if (f_gets(Buff, size, &FHandle) == NULL) 
		{
			xil_printf("Failed to read a line from foo.txt.\r\n");
			return;
		}
		xil_printf("%s", Buff);
	}
	
	f_close(&FHandle);
/*
	if (f_open(&FHandle, "bar2.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) 
	{
    	xil_printf("Failed to open bar.txt.\n\r");
		return;
	}

	if (f_write(&FHandle, "HellA!\r\n", 8, &BytesWritten) != FR_OK) 
	{
    	xil_printf("Failed to write to bar.txt.\n\r");
		return;
	}

	if (BytesWritten != 8) 
	{
		xil_printf("Wrote incorrect number of bytes to bar.txt!\n\r");
		return;
	}

	f_close(&FHandle);
	if (f_open(&FHandle, "bar2.txt", FA_READ) != FR_OK) 
	{
		xil_printf("Failed to open foo.txt.\n\r");
		return;
	}
	int size;
	size = f_size(&FHandle);
	xil_printf("size: %d \n",size);
	while (!f_eof(&FHandle)) 
	{
		if (f_gets(Buff, size, &FHandle) == NULL) 
		{
			xil_printf("Failed to read a line from bar.txt.\r\n");
			return;
		}
	
		xil_printf("%s", Buff);
	}
	
	f_close(&FHandle);
	*/
	
	xil_printf("Test Successful!\n\r");
}

FRESULT scan_files (
	char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
	
    res = f_opendir(&dir, path);							/* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);					/* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) 
			{
				break;										/* Break on error or end of dir */
			}
            if (fno.fattrib & AM_DIR)						/* It is a directory */
			{
                i = strlen(path);
                xil_printf(&path[i], "/%s", fno.fname);
                res = scan_files(path);						/* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } 
			else											/* It is a file. */
			{                                       
                xil_printf("FILE: %s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }
	
    return res;
}

int main(void) 
{
	// Initialize all peripherals and internal state.
    init_platform();
	
    if (SetupInterruptSystem() != XST_SUCCESS)
	{
    	xil_printf("Failed to initialize Interrupts.\n\r");
    	return SD_SETUP_FAILURE;
    }
	
    if (SetupSdGpio() != XST_SUCCESS) 
	{
    	xil_printf("Failed to initialize the SD card's GPIO.\n\r");
    	return SD_SETUP_FAILURE;
    }
	
    print("Platform Loaded.\n\r");
	
    // Check for presence of SD card.
    wait_for_SD_Card();
	
    xil_printf("SD card inserted.\n\r");
	
	MB_Sleep(500);
	
    //LowLevelTest();
    HighLevelTest();
	
    cleanup_platform();
    return 0;
}
