# Ultimate Converter

## Description
This project would allow you to convert an image stored on a SD Card from different file formats.

## How To Use
First, you insert a micro SD card with your image on it into the SD Card slot on the FPGA. 
The SD Card would then be read and its contents displayed on the Host PC terminal. 
After selecting the image you wished to convert (and any additional filters)
the FPGA would convert that image and apply the requested filters. 

##Repository Structure
doc - Contains our Group Report and Presentation Slides
hostPC - Code that was running on the Host PC side
legacy - Old code that is no longer in use. This includes old main files and Verilog IPs attempted.
	SPIforSD - Our original attempt to make a SD Card connection
	TestingModules - Contains old main files that tested our separate components before integration
		Conversion
		SDCard
		UART
src - Our Source code
	C - All the C Code. testparams.c contains the main function for the system that was running at the Final Demo.
		Conversion - The C Code that can covert images
		SD Card - The C Code that will access the SD Card
		UART - The C code that uses the UART
	Verilog - All the Verilog code
		IP - The Custom IP we created. The folders underneath this are the genaric folders created when an IP is generated.
		Project - The Project. It contains the main project file, and the srcs needed to build the project.
	
## Authors
Anthony De Caria
Nikita Gusev
Taylan Gocmen

## Acknowledgements
We would like to thank Professor Chow for his SD Card sample project, and the Arduino Playground for their C++ serial connection code for Windows.