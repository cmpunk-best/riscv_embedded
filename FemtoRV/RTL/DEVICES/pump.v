

module PUMP#(
  parameter COUNT = 1024
)
(
    input wire 	       clk,      // system clock
    input wire 	       rstrb,    // read strobe		
    input wire 	       wstrb,    // write strobe
    input wire         sel_pump_on, 
    input wire         sel_pump_alarm, 
    input wire         sel_pump_pressure, 
    input wire  [31:0]  wdata,    // data to be written
    output wire [31:0]  rdata    // data read
); 


localparam COUNT_WIDTH= $clog2(COUNT);

reg [COUNT_WIDTH:0] pump_pressure_count = 0;

reg pump_on = 0;

always@(posedge clk) begin 
   if(pump_on) pump_pressure_count <= (pump_pressure_count + 1'b1); //Mimiking a Pump and cut off the pump at 1024bar 
   if(sel_pump_on && wstrb) pump_on <= wdata[0] ; 
end 

wire alarm_on = wdata[0] && sel_pump_alarm && wstrb ; //Could be connected to some speaker 

reg pump_pressure = 0 ;  //Only read register 

always@(posedge clk) begin 
  if(pump_pressure_count[COUNT_WIDTH] & 1'b1) pump_pressure <= pump_pressure_count[COUNT_WIDTH] ; //Once overflows remains 1 
  if(rstrb & sel_pump_pressure) pump_pressure <= 1'b0 ; // Reset by only read 
end 

assign rdata = {30'b0,pump_pressure};

endmodule 
