
`timescale 1 ns / 1 ps

module AXI_SPI_SD_tb;
		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32;
		parameter integer C_S00_AXI_ADDR_WIDTH	= 4;

		// Inputs 
		reg  s00_axi_aclk;
		reg  s00_axi_aresetn;
		
		reg [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr;
		reg [2 : 0] s00_axi_awprot;
		reg  s00_axi_awvalid;
		reg [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata;
		reg [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb;
		reg  s00_axi_wvalid;
		
		reg  s00_axi_bready;
		reg [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr;
		reg [2 : 0] s00_axi_arprot;
		reg  s00_axi_arvalid;
		reg  s00_axi_rready;
		
		wire SD_MISO;
		
		/*
			Outputs
		*/
		// SD Wires
		wire SD_CLK;
		wire SD_CS;
		wire SD_MOSI;
		
		// Writes
		wire  s00_axi_awready;
		wire  s00_axi_wready;
		wire [1 : 0] s00_axi_bresp;
		wire  s00_axi_bvalid;
		
		// Reads
		wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata;
		wire  s00_axi_arready;
		wire [1 : 0] s00_axi_rresp;
		wire  s00_axi_rvalid;
		
		
// Instantiation of Axi Bus Interface S00_AXI
	AXI_SPI_SD_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) uut (
		.SD_CLK(SD_CLK),
		.SD_MOSI(SD_MOSI),
		.SD_MISO(SD_MISO),
		.SD_CS(SD_CS),
	
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);
	
	always begin
		#1 s00_axi_aclk = !s00_axi_aclk;
	end

	initial begin
		// Initialize Inputs
		s00_axi_aclk = 0;
		s00_axi_aresetn = 0;
		
		// Wait 100 us for global reset to finish
		#100;
        
		// Add stimulus here
		#0 s00_axi_aresetn = 1'b1;
		
		#0 s00_axi_awaddr = 4'h1;
		#0 s00_axi_awready = 1'b1;
		
		#10 s00_axi_wdata = 32'hFFFFFFFF;
		#10 s00_axi_wready = 1'b1;
		
		#50 s00_axi_awready = 1'b0;
		#50 s00_axi_wready = 1'b0;
		
		#60 s00_axi_awaddr = 4'h0;
		#60 s00_axi_awready = 1'b1;
		
		#70 s00_axi_wdata = 32'h00000002; // Write (1) = 1, CS (0) = 0
		#70 s00_axi_wready = 1'b1;
		
		#100 s00_axi_awready = 1'b0;
		#100 s00_axi_wready = 1'b0;
	end

endmodule
