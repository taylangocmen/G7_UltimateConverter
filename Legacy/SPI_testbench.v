`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/08/2017 11:39:36 AM
// Design Name: 
// Module Name: SPI_testbench
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module SPI_testbench();

    reg clk;
    reg write;
    reg [31:0]data_write;
    wire [31:0]data_read;
    reg chip_select;
    reg reset;
    wire SD_CLK;
    wire SD_MOSI;
    reg SD_MISO;
    wire SD_CS;
    
    spi_comm dut(
    .clk(clk),
    .writedata(data_write),
    .write(write),
    .chip_select(chip_select),
    .reset(reset),
    .readdata(data_read),
    .SD_CLK(SD_CLK),
    .SD_MOSI(SD_MOSI),
    .SD_MISO(SD_MISO),
    .SD_CS(SD_CS)
    );
    
    always #5 clk = ~clk;
    initial
    begin
        clk = 1'b0;
        reset = 1'b1;
        SD_MISO = 1'b0;
        data_write = 32'b0;
        chip_select = 1'b1;
        write = 1'b0;
        #20
        reset = 1'b0;
        #10
        data_write = 6'b101010;
        #10
        write = 1'b1;
        #10000
        SD_MISO = 1'b1;
        #40000
        SD_MISO =  1'b0;
        #640
        SD_MISO =  1'b1;
        #640
        SD_MISO =  1'b0;
        #640
        SD_MISO =  1'b1;
        #640
        SD_MISO =  1'b0;
        #640 
        SD_MISO =  1'b1;
        #640 
        SD_MISO =  1'b0;
        #640
        SD_MISO =  1'b1;
        
//        SD_MISO = 1'b0;
//        #10
//        SD_MISO = 1'b1;
//        #10
//        SD_MISO = 1'b0;
//        #10
//        SD_MISO = 1'b1;
//        #10
//        SD_MISO = 1'b0;
//        #10
//        SD_MISO = 1'b1;
//        #10
//        SD_MISO = 1'b0;
//        #10
//        SD_MISO = 1'b1;
       
    end
    
    
    
    
endmodule
