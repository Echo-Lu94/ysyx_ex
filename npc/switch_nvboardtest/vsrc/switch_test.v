module switch_test(
    input clk,
//    input rst,
    input a,
    input b,
    output [2:0]f
);

    assign f[2]=a^b;
    assign f[1:0]={b,a};
    endmodule
