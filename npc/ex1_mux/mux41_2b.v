module mux41_2b(
`ifdef NVBOARD
    input clk,
`endif
    input [1:0]x0,
    input [1:0]x1,
    input [1:0]x2,
    input [1:0]x3,
    input [1:0]y,
    output [1:0]f
);

MuxKey #(
    .NR_KEY(4),
    .KEY_LEN(2),
    .DATA_LEN(2)
) u_muxkey(
    .key(y),
    .out(f),
    .lut({
        2'b00,x0,
        2'b01,x1,
        2'b10,x2,
        2'b11,x3})
    );

endmodule

