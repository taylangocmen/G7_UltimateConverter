/*
 * This program just probes the SD card and checks that it is functioning, using the SPI interface.
 */

// First include the BSP headers and standard libraries.
#include "xil_printf.h"
#include "xparameters.h"
#include "xspi.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xgpio.h"
#include "microblaze_sleep.h"

// Now include project-specific headers.
#include "main.h"
#include "platform.h"
#include "diskio.h"
#include "ff.h"
#include "string.h"

// Function Declarations

int SD_file_size = 0;
void test_ip(unsigned value);

static int SetupInterruptSystem();
int SetupSdGpio();
bool SdCardInserted();

static void LowLevelTest();
static void HighLevelTest();
FRESULT scan_files (char* path);

/*
 * The instances to support the device drivers are global such that the
 * are initialized to zero each time the program runs.  They could be local
 * but should at least be static so they are zeroed.
 */
XIntc InterruptController;
XGpio SdGpio;

static TCHAR Buff[100];

volatile int* IP = XPAR_CUSTOM_SPEED_UP_FINAL_0_S00_AXI_BASEADDR;
volatile int* MEM_SD_STORE = ADDR_DDR2;

int SetupSdGpio() {
	int Status;

	// Initialize the GPIO driver. If an error occurs then exit.
	Status = XGpio_Initialize(&SdGpio, SD_GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to initialize SD GPIO driver: %d\n\r", Status);
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs.
	XGpio_SetDataDirection(&SdGpio, 1, SD_GPIO_CARD_PRESENT_MASK);

	XGpio_SelfTest(&SdGpio);

	// TODO: add Interrupt configuration code.

	return XST_SUCCESS;
}

int main() {
	// Initialize all peripherals and internal state.
    init_platform();

    if (SetupInterruptSystem() != XST_SUCCESS) {
    	xil_printf("Failed to initialize Interrupts.\n\r");
    	return -1;
    }

    if (SetupSdGpio() != XST_SUCCESS) {
    	xil_printf("Failed to initialize the SD card's GPIO.\n\r");
    	return -1;
    }

    print("Platform Loaded.\n\r");

//    wait_for_ack();

    xil_printf("Let's begin\n");

    // Check for presence of SD card.
    if (!SdCardInserted()) {
    	xil_printf("SD card not inserted! Derp!\n\r");
    	xil_printf("Please insert SD card...\n\r");
        while (!SdCardInserted()) {
        	MB_Sleep(100);
        }
    }

    xil_printf("SD card inserted.\n\r");

	//MB_Sleep(500);

    //LowLevelTest();

    //HighLevelTest();

    test_ip();

    cleanup_platform();
    return 0;
}

inline bool SdCardInserted() {
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
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the SPI can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
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

static void LowLevelTest() {
	u8 TestBuffer[2048];

    // Try to initialize the SD card.
    if (disk_initialize(0)) {
    	xil_printf("Failed to initialize SD card.\n\r");
    	return;
    }

    xil_printf("SD card initialized.\n\r");

  	if (disk_read(0, TestBuffer, 0, 2) != RES_OK) {
    	xil_printf("Failed to read first sector.\n\r");
    	return;
  	}

  	for (int i = 0; i < 128; i++) {
  		for (int j = 0; j < 4; j++) {
  			xil_printf("%02x ", TestBuffer[4 * i + j]);
  		}
  		xil_printf("\r\n");
  	}
}

static void HighLevelTest() {
	FATFS FatFs;
	FIL FHandle;
	unsigned int BytesWritten;
	FRESULT res;
	char buff[256];

	res = f_mount(&FatFs, "", 1);

	if (res == FR_OK) {
		res = scan_files(buff);
	}

//READING
	if (f_open(&FHandle, "BMP.TXT", FA_READ) != FR_OK) {
    	xil_printf("Failed to open foo.txt.\n\r");
		return;
	}
	int size;
//GET SIZE OF FILE
	size = f_size(&FHandle);
	xil_printf("size: %d \n",size);

	while (!f_eof(&FHandle)) {
		if (f_gets(Buff, 100, &FHandle) == NULL) {
			xil_printf("Failed to read a line from foo.txt.\r\n");
			return;
		}

		xil_printf("%s", Buff);
	}

	f_close(&FHandle);

	SD_file_size = size;

//END READ

//MAKE NEW FILE AND WRITE TO IT
	if (f_open(&FHandle, "tay_test.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
    	xil_printf("Failed to open bar.txt.\n\r");
		return;
	}
//WRITE THIS DATA
	if (f_write(&FHandle, "HellA!\r\n", 8, &BytesWritten) != FR_OK) {
    	xil_printf("Failed to write to bar.txt.\n\r");
		return;
	}

	if (BytesWritten != 8) {
		xil_printf("Wrote incorrect number of bytes to bar.txt!\n\r");
		return;
	}

	f_close(&FHandle);
//FINISH WRITING

//START READING
	if (f_open(&FHandle, "tay_test.txt", FA_READ) != FR_OK) {
		xil_printf("Failed to open foo.txt.\n\r");
		return;
	}
	int size;
//GET SIZE OF FILE
	size = f_size(&FHandle);
	xil_printf("size: %d \n",size);
	while (!f_eof(&FHandle)) {
		if (f_gets(Buff, size, &FHandle) == NULL) {
			xil_printf("Failed to read a line from bar.txt.\r\n");
			return;
		}

		xil_printf("%s", Buff);
	}

	f_close(&FHandle);
//FINISH READING

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


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                xil_printf(&path[i], "/%s", fno.fname);
                res = scan_files(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */

                xil_printf("FILE: %s/%s\n", path, fno.fname);
            }
        }
        f_closedir(&dir);
    }

    return res;
}
void test_ip(unsigned value){
	unsigned i;
	*(IP) = value;

   while(*(IP+1) == 0){
	   xil_printf("waiting bmp256_color_table_index_to_color: %u \n", *(IP+1));
   }
   unsigned color = *(IP+2);
   xil_printf("color = %08X \n",color);




   //write color
   *(IP+3) = color; //r=105, g = 167, b = 210
   while(*(IP+4)==0){
	   xil_printf("waiting bmp256_color_table_color_to_index: %u \n", *(IP+4));
   }
   unsigned index = *(IP+5);
   xil_printf("index = %u \n",index);
	for(i = 0; i < 4; i++) {
	   //unsigned color_table_index = i;
	   xil_printf("VALUE: %d \n", *(MEM_SD_STORE+i));
	   *(IP) = *(MEM_SD_STORE+i);

	   while(*(IP+1) == 0){
		   xil_printf("waiting bmp256_color_table_index_to_color: %u \n", *(IP+1));
	   }
	   unsigned color = *(IP+2);
	   xil_printf("color = %08X \n",color);




	   //write color
	   *(IP+3) = color; //r=105, g = 167, b = 210
	   while(*(IP+4)==0){
		   xil_printf("waiting bmp256_color_table_color_to_index: %u \n", *(IP+4));
	   }
	   unsigned index = *(IP+5);
	   xil_printf("index = %u \n",index);
	}
}
