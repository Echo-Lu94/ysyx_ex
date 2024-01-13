module rom(
//    input clk,
//    input en,
    input [7:0]addr,
    output [7:0]dout
);
wire [7:0] ROM_t [0:131];

initial begin
    $readmemh("./keycode2asic.dat",ROM_t);
end

assign dout = ROM_t[addr];

endmodule
