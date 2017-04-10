
`timescale 1 ns / 1 ps

	module custom_speed_up_FINAL_v1_0_S00_AXI #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line

		// Width of S_AXI data bus
		parameter integer C_S_AXI_DATA_WIDTH	= 32,
		// Width of S_AXI address bus
		parameter integer C_S_AXI_ADDR_WIDTH	= 5
	)
	(
		// Users to add ports here
            
		// User ports ends
		// Do not modify the ports beyond this line

		// Global Clock Signal
		input wire  S_AXI_ACLK,
		// Global Reset Signal. This Signal is Active LOW
		input wire  S_AXI_ARESETN,
		// Write address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_AWADDR,
		// Write channel Protection type. This signal indicates the
    		// privilege and security level of the transaction, and whether
    		// the transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_AWPROT,
		// Write address valid. This signal indicates that the master signaling
    		// valid write address and control information.
		input wire  S_AXI_AWVALID,
		// Write address ready. This signal indicates that the slave is ready
    		// to accept an address and associated control signals.
		output wire  S_AXI_AWREADY,
		// Write data (issued by master, acceped by Slave) 
		input wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_WDATA,
		// Write strobes. This signal indicates which byte lanes hold
    		// valid data. There is one write strobe bit for each eight
    		// bits of the write data bus.    
		input wire [(C_S_AXI_DATA_WIDTH/8)-1 : 0] S_AXI_WSTRB,
		// Write valid. This signal indicates that valid write
    		// data and strobes are available.
		input wire  S_AXI_WVALID,
		// Write ready. This signal indicates that the slave
    		// can accept the write data.
		output wire  S_AXI_WREADY,
		// Write response. This signal indicates the status
    		// of the write transaction.
		output wire [1 : 0] S_AXI_BRESP,
		// Write response valid. This signal indicates that the channel
    		// is signaling a valid write response.
		output wire  S_AXI_BVALID,
		// Response ready. This signal indicates that the master
    		// can accept a write response.
		input wire  S_AXI_BREADY,
		// Read address (issued by master, acceped by Slave)
		input wire [C_S_AXI_ADDR_WIDTH-1 : 0] S_AXI_ARADDR,
		// Protection type. This signal indicates the privilege
    		// and security level of the transaction, and whether the
    		// transaction is a data access or an instruction access.
		input wire [2 : 0] S_AXI_ARPROT,
		// Read address valid. This signal indicates that the channel
    		// is signaling valid read address and control information.
		input wire  S_AXI_ARVALID,
		// Read address ready. This signal indicates that the slave is
    		// ready to accept an address and associated control signals.
		output wire  S_AXI_ARREADY,
		// Read data (issued by slave)
		output wire [C_S_AXI_DATA_WIDTH-1 : 0] S_AXI_RDATA,
		// Read response. This signal indicates the status of the
    		// read transfer.
		output wire [1 : 0] S_AXI_RRESP,
		// Read valid. This signal indicates that the channel is
    		// signaling the required read data.
		output wire  S_AXI_RVALID,
		// Read ready. This signal indicates that the master can
    		// accept the read data and response information.
		input wire  S_AXI_RREADY
	);


    reg [4:0] counter_cti_to_c;
    reg [4:0] counter_rgb_to_i;
    reg[4:0] CurrentState;
    reg[4:0] NextState;
    wire [31:0] color;
    wire [31:0] index;
	// AXI4LITE signals
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_awaddr;
	reg  	axi_awready;
	reg  	axi_wready;
	reg [1 : 0] 	axi_bresp;
	reg  	axi_bvalid;
	reg [C_S_AXI_ADDR_WIDTH-1 : 0] 	axi_araddr;
	reg  	axi_arready;
	reg [C_S_AXI_DATA_WIDTH-1 : 0] 	axi_rdata;
	reg [1 : 0] 	axi_rresp;
	reg  	axi_rvalid;

	// Example-specific design signals
	// local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
	// ADDR_LSB is used for addressing 32/64 bit registers/memories
	// ADDR_LSB = 2 for 32 bits (n downto 2)
	// ADDR_LSB = 3 for 64 bits (n downto 3)
	localparam integer ADDR_LSB = (C_S_AXI_DATA_WIDTH/32) + 1;
	localparam integer OPT_MEM_ADDR_BITS = 2;
	//----------------------------------------------
	//-- Signals for user logic register space example
	//------------------------------------------------
	//-- Number of Slave Registers 7
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg0;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg1;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg2;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg3;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg4;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg5;
	reg [C_S_AXI_DATA_WIDTH-1:0]	slv_reg6;
	wire	 slv_reg_rden;
	wire	 slv_reg_wren;
	reg [C_S_AXI_DATA_WIDTH-1:0]	 reg_data_out;
	integer	 byte_index;

	// I/O Connections assignments

	assign S_AXI_AWREADY	= axi_awready;
	assign S_AXI_WREADY	= axi_wready;
	assign S_AXI_BRESP	= axi_bresp;
	assign S_AXI_BVALID	= axi_bvalid;
	assign S_AXI_ARREADY	= axi_arready;
	assign S_AXI_RDATA	= axi_rdata;
	assign S_AXI_RRESP	= axi_rresp;
	assign S_AXI_RVALID	= axi_rvalid;
	// Implement axi_awready generation
	// axi_awready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is
	// de-asserted when reset is low.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awready <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID)
	        begin
	          // slave is ready to accept write address when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_awready <= 1'b1;
	        end
	      else           
	        begin
	          axi_awready <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_awaddr latching
	// This process is used to latch the address when both 
	// S_AXI_AWVALID and S_AXI_WVALID are valid. 

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_awaddr <= 0;
	    end 
	  else
	    begin    
	      if (~axi_awready && S_AXI_AWVALID && S_AXI_WVALID)
	        begin
	          // Write Address latching 
	          axi_awaddr <= S_AXI_AWADDR;
	        end
	    end 
	end       

	// Implement axi_wready generation
	// axi_wready is asserted for one S_AXI_ACLK clock cycle when both
	// S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is 
	// de-asserted when reset is low. 

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_wready <= 1'b0;
	    end 
	  else
	    begin    
	      if (~axi_wready && S_AXI_WVALID && S_AXI_AWVALID)
	        begin
	          // slave is ready to accept write data when 
	          // there is a valid write address and write data
	          // on the write address and data bus. This design 
	          // expects no outstanding transactions. 
	          axi_wready <= 1'b1;
	        end
	      else
	        begin
	          axi_wready <= 1'b0;
	        end
	    end 
	end       

	// Implement memory mapped register select and write logic generation
	// The write data is accepted and written to memory mapped registers when
	// axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to
	// select byte enables of slave registers while writing.
	// These registers are cleared when reset (active low) is applied.
	// Slave register write enable is asserted when valid address and data are available
	// and the slave is ready to accept the write address and write data.
	assign slv_reg_wren = axi_wready && S_AXI_WVALID && axi_awready && S_AXI_AWVALID;

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      slv_reg0 <= 0;
	      //slv_reg1 <= 0;
	      slv_reg2 <= 0;
	      slv_reg3 <= 0;
	      //slv_reg4 <= 0;
	      slv_reg5 <= 0;
	      slv_reg6 <= 0;
	    end 
	  else begin
	    if (slv_reg_wren)
	      begin
	        case ( axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	          3'h0:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 0
	                slv_reg0[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          /*3'h1:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 1
	                slv_reg1[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          3'h2:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 2
	                slv_reg2[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  */
	          3'h3:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 3
	                slv_reg3[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          /*3'h4:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 4
	                slv_reg4[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          3'h5:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 5
	                slv_reg5[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  */
	          3'h6:
	            for ( byte_index = 0; byte_index <= (C_S_AXI_DATA_WIDTH/8)-1; byte_index = byte_index+1 )
	              if ( S_AXI_WSTRB[byte_index] == 1 ) begin
	                // Respective byte enables are asserted as per write strobes 
	                // Slave register 6
	                slv_reg6[(byte_index*8) +: 8] <= S_AXI_WDATA[(byte_index*8) +: 8];
	              end  
	          default : begin
	                      slv_reg0 <= slv_reg0;
	                      slv_reg1 <= slv_reg1;
	                      slv_reg2 <= slv_reg2;
	                      slv_reg3 <= slv_reg3;
	                      slv_reg4 <= slv_reg4;
	                      slv_reg5 <= slv_reg5;
	                      slv_reg6 <= slv_reg6;
	                    end
	        endcase
	      end
	  end
	end    

	// Implement write response logic generation
	// The write response and response valid signals are asserted by the slave 
	// when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.  
	// This marks the acceptance of address and indicates the status of 
	// write transaction.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_bvalid  <= 0;
	      axi_bresp   <= 2'b0;
	    end 
	  else
	    begin    
	      if (axi_awready && S_AXI_AWVALID && ~axi_bvalid && axi_wready && S_AXI_WVALID)
	        begin
	          // indicates a valid write response is available
	          axi_bvalid <= 1'b1;
	          axi_bresp  <= 2'b0; // 'OKAY' response 
	        end                   // work error responses in future
	      else
	        begin
	          if (S_AXI_BREADY && axi_bvalid) 
	            //check if bready is asserted while bvalid is high) 
	            //(there is a possibility that bready is always asserted high)   
	            begin
	              axi_bvalid <= 1'b0; 
	            end  
	        end
	    end
	end   

	// Implement axi_arready generation
	// axi_arready is asserted for one S_AXI_ACLK clock cycle when
	// S_AXI_ARVALID is asserted. axi_awready is 
	// de-asserted when reset (active low) is asserted. 
	// The read address is also latched when S_AXI_ARVALID is 
	// asserted. axi_araddr is reset to zero on reset assertion.

	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_arready <= 1'b0;
	      axi_araddr  <= 32'b0;
	    end 
	  else
	    begin    
	      if (~axi_arready && S_AXI_ARVALID)
	        begin
	          // indicates that the slave has acceped the valid read address
	          axi_arready <= 1'b1;
	          // Read address latching
	          axi_araddr  <= S_AXI_ARADDR;
	        end
	      else
	        begin
	          axi_arready <= 1'b0;
	        end
	    end 
	end       

	// Implement axi_arvalid generation
	// axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both 
	// S_AXI_ARVALID and axi_arready are asserted. The slave registers 
	// data are available on the axi_rdata bus at this instance. The 
	// assertion of axi_rvalid marks the validity of read data on the 
	// bus and axi_rresp indicates the status of read transaction.axi_rvalid 
	// is deasserted on reset (active low). axi_rresp and axi_rdata are 
	// cleared to zero on reset (active low).  
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rvalid <= 0;
	      axi_rresp  <= 0;
	    end 
	  else
	    begin    
	      if (axi_arready && S_AXI_ARVALID && ~axi_rvalid)
	        begin
	          // Valid read data is available at the read data bus
	          axi_rvalid <= 1'b1;
	          axi_rresp  <= 2'b0; // 'OKAY' response
	        end   
	      else if (axi_rvalid && S_AXI_RREADY)
	        begin
	          // Read data is accepted by the master
	          axi_rvalid <= 1'b0;
	        end                
	    end
	end    

	// Implement memory mapped register select and read logic generation
	// Slave register read enable is asserted when valid address is available
	// and the slave is ready to accept the read address.
	assign slv_reg_rden = axi_arready & S_AXI_ARVALID & ~axi_rvalid;
	always @(*)
	begin
	      // Address decoding for reading registers
	      case ( axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB] )
	        3'h0   : reg_data_out <= slv_reg0;
	        3'h1   : reg_data_out <= slv_reg1;
	        3'h2   : reg_data_out <= color;
	        3'h3   : reg_data_out <= slv_reg3;
	        3'h4   : reg_data_out <= slv_reg4;
	        3'h5   : reg_data_out <= index;
	        3'h6   : reg_data_out <= slv_reg6;
	        default : reg_data_out <= 0;
	      endcase
	end

	// Output register or memory read data
	always @( posedge S_AXI_ACLK )
	begin
	  if ( S_AXI_ARESETN == 1'b0 )
	    begin
	      axi_rdata  <= 0;
	    end 
	  else
	    begin    
	      // When there is a valid read address (S_AXI_ARVALID) with 
	      // acceptance of read address by the slave (axi_arready), 
	      // output the read dada 
	      if (slv_reg_rden)
	        begin
	          axi_rdata <= reg_data_out;     // register read data
	        end   
	    end
	end    

	// Add user logic here
	

	localparam STATE_IDLE = 5'd0,
	           STATE_1 = 5'd1,
	           STATE_2 = 5'd2,
	           STATE_3 = 5'd3,
	           STATE_4 = 5'd4,
	           STATE_5 = 5'd5;
	 
	   
	 
	 always@(posedge S_AXI_ACLK)
	 begin
	   //CurrentState <= NextState;
        if ( S_AXI_ARESETN == 1'b0 )
             begin
               CurrentState <= STATE_IDLE;
             end 
        else
            begin
                CurrentState <= NextState;
            end
	 end
	 wire wrote_to_slv_reg_0 = (axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB]==3'h0) ? 1'b1:1'b0;
	 wire wrote_to_slv_reg_3 = (axi_awaddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB]==3'h3) ? 1'b1:1'b0;
	 wire read_slv_reg_2 = (axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB]==3'h2) ? 1'b1: 1'b0;
	 wire read_slv_reg_5 = (axi_araddr[ADDR_LSB+OPT_MEM_ADDR_BITS:ADDR_LSB]==3'h5) ? 1'b1: 1'b0;
	 always@(*)begin
	       //NextState = CurrentState;
	       case(CurrentState)
	           STATE_IDLE: begin
	               //selected and wrote data to slv_reg0 (color table index)
	               if(slv_reg_wren && (wrote_to_slv_reg_0))begin
	                   NextState = STATE_1;
	               
	               end
	               //selected and wrote data to slv_reg3 (rgb)
	               else if(slv_reg_wren && (wrote_to_slv_reg_3))begin
                       NextState = STATE_3;
                  
                   end
	               
	               else     NextState = STATE_IDLE;
	           end
	           //wait 7 cycles for index to get processed into color table
	           STATE_1: begin
	               if(counter_cti_to_c < 5'd7) begin
	                   NextState = STATE_1;
	               end
	               else NextState = STATE_2;
	           end
	           //finished processing index, write 1 to slv_reg1 (COMPLETE)
	           //write answer into slv_reg2 (done automatically)
	           
	           //wait for user to read from slv_reg2
	           STATE_2: begin
	               if(read_slv_reg_2)begin
	                   NextState = STATE_IDLE;
	               end
	               else NextState = STATE_2;
	           end
	           
	           //rgb to index function
	           STATE_3: begin
	               if(counter_rgb_to_i < 5'd7) begin
                       NextState = STATE_3;
                   end
                   else NextState = STATE_4;
	           end
	           //wait for user to read slv_reg5
	           STATE_4: begin
	               if(read_slv_reg_5)begin
                       NextState = STATE_IDLE;
                   end
                   else NextState = STATE_4;
	           
	           end
	           
	           
	           default: begin
	               NextState = STATE_IDLE;
	           end
           endcase
	 
	 end
	 
	 always@(posedge S_AXI_ACLK)begin
	   case(CurrentState)
	       STATE_IDLE: begin
	           slv_reg1 = 0; //index to color status
	           slv_reg4 = 0; //color to index status
	           counter_cti_to_c = 5'd0;
	           counter_rgb_to_i = 5'd0;
	       end
	       STATE_1: begin
	           counter_cti_to_c = counter_cti_to_c + 1'd1;
	           counter_rgb_to_i = 5'd0;
	           slv_reg1 = 1'b0; //index to color status
	           slv_reg4 = 1'b0; //color to index status
	       end
	       STATE_2: begin
	           counter_cti_to_c = 5'd0;
	           counter_rgb_to_i = 5'd0;
	           slv_reg1 = 32'd1; //color from index COMPLETE
	           slv_reg4 = 0; //color to index status
	       end
	       STATE_3: begin
               counter_cti_to_c = 5'd0;
               counter_rgb_to_i = counter_rgb_to_i + 1'd1;
               slv_reg1 = 0; //index to color status
               slv_reg4 = 0; //color to index status
           end
           STATE_4: begin
               counter_cti_to_c = 5'd0;
               counter_rgb_to_i = 5'd0;
               slv_reg1 = 0; //index to color status
               slv_reg4 = 32'd1; //color to index COMPLETE
           end
	       default: begin
	           counter_cti_to_c = 5'd0;
               counter_rgb_to_i = 5'd0;
               slv_reg1 = 0; //index to color status
               slv_reg4 = 0; //color to index COMPLETE
	       
	       
	       end
	       
	   endcase


	 end    
	       
        bmp256_color_table_index_to_color index_to_color(
             .i(slv_reg0),
             .clk(S_AXI_ACLK),
             .color(color));
         
         bmp256_color_table_color_to_index color_to_index(
             .rgb(slv_reg3),
             .clk(S_AXI_ACLK),
             .index(index));
    
	// User logic ends

	endmodule
 module bmp256_color_table_color_to_index(
           input [31:0] rgb,
           input clk,
           output reg [31:0] index);
   
           reg [31:0]blue;
           reg [31:0]green;
           reg [31:0]red;
           wire [31:0]r;
           wire [31:0]g;
           wire [31:0]b;
           assign b[31:0] = {24'd0,rgb[7:0]};
           assign g[31:0] = {24'd0,rgb[15:8]};
           assign r[31:0] = {24'd0,rgb[23:16]};
   
           parameter integer BMP_256 = 256; 
           parameter integer FACTBLUE = 6; 
           parameter integer FACTGREEN = 6; 
           parameter integer FACTRED = 6;
           
           always@(posedge clk)begin
                   if((r%32'd51)>=32'd26) begin
                       red <= r/(32'd51) + 32'd1;
                   end
                   else begin
                       red <= r/(32'd51);
                   end
                   if((g%32'd51)>=32'd26) begin
                       green <= g/(32'd51) + 32'd1;
                   end
                   else begin
                       green <= g/(32'd51);
                   end
                   if((b%32'd51)>=32'd26) begin
                       blue <= b/(32'd51) + 32'd1;
                   end
                   else begin
                       blue <= b/(32'd51);
                   end
                   
                   index <= (red*(FACTBLUE * FACTGREEN)) + (green*FACTBLUE) + blue; 
           end
           
           
   
   endmodule
   
   module bmp256_color_table_index_to_color(
           input [31:0] i,
           input clk,
           output reg [31:0] color);
           
           parameter integer BMP_256 = 256; 
           parameter integer FACTBLUE = 6; 
           parameter integer FACTGREEN = 6; 
           parameter integer FACTRED = 6;
       
           reg [31:0]blue;
           reg [31:0]green;
           reg [31:0]red;
       
           
           always@(posedge clk)begin
               blue <= i % FACTBLUE;
               green <= (i/ FACTBLUE) % FACTGREEN; 
               red <= (i / (32'd36)) % FACTRED; 
               color <= (((red * 32'd51)*BMP_256+(green * 32'd51))*BMP_256)+(blue * 32'd51); 
           end
    
    endmodule