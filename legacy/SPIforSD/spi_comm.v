`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/07/2017 10:51:42 AM
// Design Name: 
// Module Name: spi_comm
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


module spi_comm(
    input clk,
    input [31:0]writedata,
    input write,
    input chip_select,
    input reset, //reset = 1 for reseting
    output [31:0]readdata,
    output SD_CLK,
    output reg SD_MOSI,
    input SD_MISO,
    output reg SD_CS
    );
    
    //cnt will divide the clock by 256 making is under 400KHz
    reg [7:0] cnt;
    initial cnt = 8'b0;
    always@(posedge clk) begin
        cnt <= cnt+1;
    end
    
    reg clock_slow;
    reg write_cmd_0;
    reg cmd_0_read;
    reg [6:0] initial_counter;
    initial initial_counter = 7'd10; //CHANGE BACK TO 100!!!!!!!!!!!!!!!! only changed to 10 for simulation
    reg initial_done;
    reg shifting;
    reg reading;
    initial shifting = 1'b0;
    initial reading = 1'b0;
    
    //writing regs
    reg [47:0] data_out;
    //reg [5:0] command;
    wire [5:0] command;
    assign command = writedata[5:0];
    reg [31:0] argument;
    initial argument = 32'b0; //FOR SIMULATION
    reg [6:0] CRC;
    initial CRC = 7'b1111110; //FOR SIMULATION
    //reading regs
    reg [7:0] read_byte;
    initial read_byte = 8'b0;
    
    reg [6:0] write_counter;
    initial write_counter = 7'd47;
    reg [3:0] read_counter;
    initial read_counter = 4'd8;
    reg [3:0] toggle_counter;
    initial toggle_counter = 4'd8;
    reg toggling;
    //initial data_temp = {{2'b01},{
    //clock_slow will be used to switch between fast/slow 
    //clock for the SD card
    assign SD_CLK = cnt[5];
    
    
    localparam STATE_Initial = 5'd0,
               STATE_write = 5'd1,
               STATE_read = 5'd2,
               STATE_idle = 5'd3,
               STATE_toggling = 5'd4,
               STATE_waiting_for_response = 5'd5;
            
               
    reg [4:0] CurrentState;
    reg [4:0] NextState;
    
    always@(posedge clk)begin
        if(reset) CurrentState <= STATE_Initial;
        else CurrentState <= NextState;
    end
    
    always@(*)begin
        NextState = CurrentState;
        case(CurrentState)
            STATE_Initial: begin
                if(initial_done == 1) NextState = STATE_idle;
                else NextState = STATE_Initial;
            end
            STATE_idle: begin
                //check if there is something avaialbe to write and the card is ready to be written to
                if((write == 1)&&(SD_MISO==1)) NextState = STATE_write;
                else NextState = STATE_idle;
            end
            STATE_write: begin
                if(write_counter > 0) NextState = STATE_write;
                else NextState = STATE_waiting_for_response;
            end
            /*STATE_toggling: begin
                if(toggle_counter >0) NextState = STATE_toggling;
                else NextState = STATE_read;
            end*/
            //WAIT FOR MISO TO GO LOW, then start reading
            STATE_waiting_for_response: begin
                if(SD_MISO == 1'b1) NextState =STATE_waiting_for_response;
                else NextState = STATE_read;
            end
            STATE_read: begin
                if(read_counter > 0) NextState = STATE_read;
                else NextState = STATE_idle;
                
            end
            default: begin
                NextState = STATE_idle;
            end
        endcase
    end
    always@(*)begin
        case(CurrentState)
            STATE_Initial: begin
                clock_slow = 1'b1;
                write_cmd_0 = 1'b1;
                shifting = 1'b0;
                reading = 1'b0;
                toggling = 1'b0;
            end
            STATE_write: begin
                shifting = 1'b1;
                reading = 1'b0;
                toggling = 1'b0;
            end
            STATE_waiting_for_response: begin
                shifting = 1'b0;
                reading = 1'b0;
                toggling = 1'b0;
            end
            /*STATE_toggling: begin
                shifting = 1'b0;
                reading = 1'b0;
                toggling = 1'b1;
            end*/
            STATE_read: begin
                reading = 1'b1;
                shifting = 1'b0;
                toggling = 1'b0;
            end
            STATE_idle: begin
                shifting = 1'b0;
                reading = 1'b0;
                toggling = 1'b0;
            end
            default: begin
                shifting = 1'b0;
                reading = 1'b0;
                toggling = 1'b0;
            end
        endcase     
    end
    
    //writing
    always@(posedge SD_CLK)begin
        //shift data 1 by 1
        if(shifting)begin
            data_out <= {data_out[46:0],1'b0};
            write_counter <= write_counter - 1'd1;
        end
        else begin
            data_out[47:0] <= {{2'b01},{command[5:0]},{argument[31:0]},{CRC[6:0]},{1'b1}};
            write_counter <= 7'd47;
        end
    end
    //reading
    always@(posedge SD_CLK)begin
        if(reading)begin
            read_counter <= read_counter - 1'd1;
            read_byte <= {read_byte[6:0],SD_MISO};
        end
        else begin
            read_counter <= 4'd8;
        end
    end
    //wait 8 cycles before reading 
    //NOT NECESSARY JUST HAVE TO WAIT FOR MISO TO GO LOW
    /*always@(posedge SD_CLK)begin
        if(toggling)begin
            toggle_counter = toggle_counter - 1'd1;
        end
        else begin
            toggle_counter = 4'd8;
        end
    end*/
    
    //sets clock to the SD card for initialization for 100 cycles
    always@(posedge SD_CLK)begin
        if(initial_counter>0)begin
            initial_done <=0;
            initial_counter <=initial_counter-1'd1;
        end
        else initial_done<=1;
    end
    
    //assign SD_MOSI = (shifting||toggling) ? data_out[47] : 1'bZ; //SUSPISIOUSS OR!!! 
    always@(posedge SD_CLK)begin
        if(shifting) SD_MOSI <= data_out[47];
        else if(toggling) SD_MOSI <= 1'b1;
        else SD_MOSI <= 1'b1; // by default MOSI is set to 1 showing there is no message
    end
    //assign SD_CS = (CurrentState == STATE_idle)||(CurrentState == STATE_Initial) ? 1'b1 : 1'b0; //active low signal, assert chipselect when NOT in idel state
    always@(posedge SD_CLK)begin
        if((CurrentState == STATE_idle)||(CurrentState == STATE_Initial))begin
            SD_CS <= 1'b1;
        end
        else SD_CS <= 0'b0;
    end
    
    
endmodule
